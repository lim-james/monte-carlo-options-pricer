#include "monte_carlo.h"

#include <random>
#include <algorithm>
#include <cmath>

double grow(const eu_option& option) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<> dist(0.0, 1.0);

    double Z = dist(gen);

    double a = option.rate - 0.5 * option.volatility * option.volatility;
    double b = option.volatility * std::sqrt(option.expiry) * Z;
    double c = std::exp(a * option.expiry + b);
     
    return option.spot * c;
}

double discount(double future_price, double rate, double time_to_expiry) {
    return future_price * exp(-rate * time_to_expiry);
}

double payoff_call(const eu_option& option) {
    double future_price = grow(option);
    double payoff = std::max(future_price - option.strike, 0.0);
    double discounted_payoff = discount(payoff, option.rate, option.expiry);
    return discounted_payoff;
}

double monte_carlo_call_pricing(const eu_option& option, const monte_carlo_parameters& params) {
    double sample_fraction = 1.0 / params.sample_count;
    double avg_payoff = 0.0;
    for (size_t i = 0; i < params.sample_count; ++i) {
        double payoff = payoff_call(option);
        avg_payoff += payoff * sample_fraction;
    }
    return avg_payoff;
}
