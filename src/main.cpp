#include <print>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <string>

#include "rapidcsv.h"
#include "monte_carlo.h"
#include "black_scholes.h"
#include "io.h"


int main(int argsc, const char* argsv[]) {
    if (argsc <= 1) {
        std::println("Please provide a .csv filepath");
        return 0;

    }
    auto eu_options_list = load_options_from_csv(argsv[1]);

    const uint32_t sample_count = argsc > 2 ? std::stol(argsv[2]) : 1000000;
    const monte_carlo_parameters params{sample_count};

    std::println("{} options // {} samples", eu_options_list.size(), params.sample_count);

    for (const eu_option& option: eu_options_list) {
        auto start = std::chrono::high_resolution_clock::now();
        const double mc_payoff = monte_carlo_call_pricing(option, params);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto mc_dt = duration_cast<std::chrono::milliseconds>(end - start);

        start = std::chrono::high_resolution_clock::now();
        const double bs_payoff = black_scholes_call_pricing(option);
        end = std::chrono::high_resolution_clock::now();

        auto bs_dt = duration_cast<std::chrono::milliseconds>(end - start);

        std::println("MC payoff = {:.02f} [{}]", mc_payoff, mc_dt); 
        std::println("BS payoff = {:.02f} [{}]", bs_payoff, bs_dt); 
        std::println("|MC - BS| = {:.02f}", mc_payoff - bs_payoff); 
    }

    return 0;
}
