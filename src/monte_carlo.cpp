#include "monte_carlo.h"

#include <algorithm>
#include <cmath>
#include <thread>
#include <functional>
#include <ranges>

#define TEST_ALIGNED


double grow(const eu_option& option, std::mt19937& gen) {
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

double payoff(const eu_option& option, std::mt19937& gen) {
    double future_price = grow(option, gen);
    double raw_payoff = option.type == OptionType::Call 
        ? std::max(future_price - option.strike, 0.0)
        : std::max(option.strike - future_price, 0.0);
    double discounted_payoff = discount(raw_payoff, option.rate, option.expiry);
    return discounted_payoff;
}

inline unsigned int thread_seed(unsigned int seed, unsigned int thread_id) {
    return seed + 31 * thread_id;
}

void monte_carlo_pricing_batch(
    const eu_option& option, 
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

double monte_carlo_pricing(
    const eu_option& option, 
    const monte_carlo_parameters& params,
    const unsigned int seed
) {
#ifdef TEST_ALIGNED
    std::vector<aligned_double> avg_payoffs(params.thread_count); 
#else
    std::vector<double> avg_payoffs(params.thread_count); 
#endif

    {
        auto thread_dispatch = [&](unsigned int i) {
            return std::jthread(
                monte_carlo_pricing_batch, 
                std::ref(option), 
                params.sample_count / params.thread_count,
#ifdef TEST_ALIGNED
                &(avg_payoffs[i].x),
#else
                &avg_payoffs[i],
#endif
                seed,
                i
            );
        };
        std::ranges::for_each(
            std::views::iota(0u, params.thread_count), 
            thread_dispatch
        );
    }


    double sample_fraction = 1.0 / params.thread_count;
    double avg_payoff = 0.0;
    for (unsigned int i = 0; i < params.thread_count; ++i) {
#ifdef TEST_ALIGNED
        avg_payoff += avg_payoffs[i].x * sample_fraction;
#else
        avg_payoff += avg_payoffs[i] * sample_fraction;
#endif
    }

    return avg_payoff;
}
