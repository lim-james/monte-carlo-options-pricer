#include "pricer/method/blackscholes.h"

#include <cmath>

namespace pricer {
namespace method::blackscholes {

inline double cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2));
}

double BlackScholesPricer::price(const model::EuropeanOption& option) const {
    double d1_num_ln = std::log(option.spot / option.strike);
    double d1_num_rate = option.risk_free_rate + 0.5 * option.implied_volatility * option.implied_volatility;
    double d1_num = d1_num_ln + d1_num_rate * option.expiry;
    double d1_den = option.implied_volatility * std::sqrt(option.expiry);
    double d1 = d1_num / d1_den;

    double d2 = d1 - d1_den;

    double present_strike_value_e = std::exp(-option.risk_free_rate * option.expiry);

    if (option.type == model::OptionType::Call) {
        double value_at_expiry = option.spot * cdf(d1);
        double present_strike_value = option.strike * present_strike_value_e * cdf(d2);
        return value_at_expiry - present_strike_value;
    } else {
        double value_at_expiry = option.spot * cdf(-d1);
        double present_strike_value = option.strike * present_strike_value_e * cdf(-d2);
        return present_strike_value - value_at_expiry;
    }
}

}
}

