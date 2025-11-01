#include "monte_carlo.h"

#include <random>
#include <algorithm>
#include <cmath>
#include <thread>
#include <functional>

struct alignas(64) aligned_double {
    double x;
};

double grow(const eu_option& option) {
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    thread_local std::normal_distribution<> dist(0.0, 1.0);

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

void monte_carlo_call_pricing_batch(
    const eu_option& option, 
    size_t batch_size,
    aligned_double* payoff_mem
) {
    double batch_fraction = 1.0 / batch_size;
    double avg_payoff = 0.0;
    for (size_t i = 0; i < batch_size; ++i) {
        double payoff = payoff_call(option);
        avg_payoff += payoff * batch_fraction;
    }
    payoff_mem->x = avg_payoff;
}

double monte_carlo_call_pricing(const eu_option& option, const monte_carlo_parameters& params) {
    std::vector<aligned_double> avg_payoffs(params.thread_count); 
    std::vector<std::thread> threads; 
    threads.reserve(params.thread_count);

    for (unsigned int i = 0; i < params.thread_count; ++i) {
        threads.emplace_back(
            monte_carlo_call_pricing_batch, 
            std::ref(option), 
            params.sample_count / params.thread_count,
            &avg_payoffs[i]
        );
    }

    for (auto it = threads.rbegin(); it != threads.rend(); ++it) {
        it->join();
    }

    double sample_fraction = 1.0 / params.thread_count;
    double avg_payoff = 0.0;
    for (unsigned int i = 0; i < params.thread_count; ++i) {
        avg_payoff += avg_payoffs[i].x * sample_fraction;
    }

    return avg_payoff;
}
