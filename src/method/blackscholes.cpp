#include "pricer/method/blackscholes.h"

#include <cmath>

namespace pricer {
namespace method::blackscholes {

inline double cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2));
}

double BlackScholesPricer::price(const model::EuropeanOption& option) const {
    const double d1_num_ln = std::log(option.spot / option.strike);
    const double d1_num_rate = option.rate + 0.5 * option.volatility * option.volatility;
    const double d1_num = d1_num_ln + d1_num_rate * option.expiry;
    const double d1_den = option.volatility * std::sqrt(option.expiry);
    const double d1 = d1_num / d1_den;

    const double d2 = d1 - d1_den;

    const double present_strike_value_e = std::exp(-option.rate * option.expiry);

    if (option.type == model::OptionType::Call) {
        const double value_at_expiry = option.spot * cdf(d1);
        const double present_strike_value = option.strike * present_strike_value_e * cdf(d2);
        return value_at_expiry - present_strike_value;
    } else {
        const double value_at_expiry = option.spot * cdf(-d1);
        const double present_strike_value = option.strike * present_strike_value_e * cdf(-d2);
        return present_strike_value - value_at_expiry;
    }
}

}
}

