#include <print>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>

#include "eu_option.h" 
#include "monte_carlo_parameters.h" 

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

double cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2));
}

double black_scholes_call_pricing(const eu_option& option) {
    const double d1_num_ln = std::log(option.spot / option.strike);
    const double d1_num_rate = option.rate + 0.5 * option.volatility * option.volatility;
    const double d1_num = d1_num_ln + d1_num_rate * option.expiry;
    const double d1_den = option.volatility * std::sqrt(option.expiry);
    const double d1 = d1_num / d1_den;

    const double value_at_expiry = option.spot * cdf(d1);

    const double d2 = d1 - d1_den;

    const double present_strike_value_e = std::exp(-option.rate * option.expiry);
    const double present_strike_value = option.strike * present_strike_value_e * cdf(d2);

    return value_at_expiry - present_strike_value;
}

int main(int argsc, const char* argsv[]) {
    const double spot_price = 100;
    const double strike_price = 100;
    const double time_to_expiry = 1.0;
    const double risk_free_rate = 0.05;
    const double volatility = 0.2;

    const eu_option op{spot_price, strike_price, time_to_expiry, volatility, risk_free_rate};
    const monte_carlo_parameters params{1000000};

    auto start = std::chrono::high_resolution_clock::now();
    const double mc_payoff = monte_carlo_call_pricing(op, params);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto mc_dt = duration_cast<std::chrono::milliseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();
    const double bs_payoff = black_scholes_call_pricing(op);
    end = std::chrono::high_resolution_clock::now();

    auto bs_dt = duration_cast<std::chrono::milliseconds>(end - start);

    std::println("MC payoff = {:.02f} [{}][{} samples]", mc_payoff, mc_dt, params.sample_count); 
    std::println("BS payoff = {:.02f} [{}]", bs_payoff, bs_dt); 
    std::println("|MC - BS| = {:.02f}", mc_payoff - bs_payoff); 

    return 0;
}
