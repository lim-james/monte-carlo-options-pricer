#include <print>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <string>

#include "monte_carlo.h"
#include "black_scholes.h"

int main(int argsc, const char* argsv[]) {
    const double spot_price = 100;
    const double strike_price = 100;
    const double time_to_expiry = 1.0;
    const double risk_free_rate = 0.05;
    const double volatility = 0.4;

    const eu_option op{spot_price, strike_price, time_to_expiry, volatility, risk_free_rate};

    const uint32_t sample_count = argsc > 1 ? std::stoi(argsv[1]) : 1000000;
    const monte_carlo_parameters params{sample_count};

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
