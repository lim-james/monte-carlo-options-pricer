#include "pricer/method/montecarlo.h"

#include <algorithm>
#include <cmath>
#include <thread>
#include <ranges>
#include <random>

namespace pricer {
namespace method::montecarlo {

double grow(const model::EuropeanOption& option, std::mt19937& gen) {
    thread_local std::normal_distribution<> dist(0.0, 1.0);

    const double Z = dist(gen);
    const double a = option.rate - 0.5 * option.volatility * option.volatility;
    const double b = option.volatility * std::sqrt(option.expiry) * Z;
    const double c = std::exp(a * option.expiry + b);
     
    return option.spot * c;
}

inline double discount(double future_price, double rate, double time_to_expiry) {
    return future_price * exp(-rate * time_to_expiry);
}

double payoff(const model::EuropeanOption& option, std::mt19937& gen) {
    double future_price = grow(option, gen);
    double raw_payoff = option.type == model::OptionType::Call 
        ? std::max(future_price - option.strike, 0.0)
        : std::max(option.strike - future_price, 0.0);
    double discounted_payoff = discount(raw_payoff, option.rate, option.expiry);
    return discounted_payoff;
}

inline unsigned int thread_seed(unsigned int seed, unsigned int thread_id) {
    return seed + 31 * thread_id;
}

void monte_carlo_pricing_batch(
    const model::EuropeanOption& option, 
    size_t batch_size,
    double* payoff_mem,
    unsigned int seed,
    unsigned int thread_id
) {
    std::mt19937 gen(thread_seed(seed, thread_id));

    auto payoff_branch = [&option, &gen](auto) { return payoff(option, gen); };
    auto payoffs = std::views::repeat(0U, batch_size) 
                 | std::views::transform(payoff_branch);
    double total_payoff = std::ranges::fold_left(payoffs, 0, std::plus{});

    *payoff_mem = total_payoff / (double)batch_size;
}

#ifdef TEST_ALIGNED
struct alignas(64) aligned_double {
    double x;
};
#endif

MonteCarloPricer::MonteCarloPricer(
    const unsigned int simulation_seed,
    const model::MontecarloParameters& params
) : simulation_seed_(simulation_seed), simulation_parameters_(params) {}

double MonteCarloPricer::price(
    const model::EuropeanOption& option
) const {
#ifdef TEST_ALIGNED
    std::vector<aligned_double> avg_payoffs(params.thread_count); 
#else
    std::vector<double> avg_payoffs(simulation_parameters_.thread_count); 
#endif

    {
        std::vector<std::jthread> threads;
        threads.reserve(simulation_parameters_.thread_count);

        for (auto i : std::views::iota(0u, simulation_parameters_.thread_count))
            threads.emplace_back(
                monte_carlo_pricing_batch, 
                std::ref(option), 
                simulation_parameters_.sample_count / simulation_parameters_.thread_count,
#ifdef TEST_ALIGNED
                &(avg_payoffs[i].x),
#else
                &avg_payoffs[i],
#endif
                simulation_seed_,
                i
            );
    }

#ifdef TEST_ALIGNED
    double total_payoff = 0.0;
    for (unsigned int i = 0; i < params.thread_count; ++i) {
        total_payoff += avg_payoffs[i].x;
    }
#else
    double total_payoff = std::accumulate(avg_payoffs.begin(), avg_payoffs.end(), 0.0);
#endif

    return total_payoff / (double)simulation_parameters_.thread_count;
}

