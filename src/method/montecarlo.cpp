#include "pricer/method/montecarlo.h"

#include <functional>
#include <algorithm>
#include <cmath>
#include <thread>
#include <ranges>
#include <random>

namespace pricer {
namespace method::montecarlo {

double calculate_future_price(const model::EuropeanOption& option, std::mt19937& gen) {
    thread_local std::normal_distribution<> dist(0.0, 1.0);

    // TODO: figure out better names
    double Z = dist(gen);
    double a = option.rate - 0.5 * option.volatility * option.volatility;
    double b = option.volatility * std::sqrt(option.expiry) * Z;
    double c = std::exp(a * option.expiry + b);
     
    return option.spot * c;
}

inline double apply_discount(double future_price, double market_rate, double time_to_expiry) {
    return future_price * std::exp(-market_rate * time_to_expiry);
}

double calculate_discounted_payoff(const model::EuropeanOption& option, std::mt19937& gen) {
    double future_price = calculate_future_price(option, gen);
    double raw_payoff = option.type == model::OptionType::Call 
        ? std::max(future_price - option.strike, 0.0)
        : std::max(option.strike - future_price, 0.0);
    double discounted_payoff = apply_discount(raw_payoff, option.rate, option.expiry);
    return discounted_payoff;
}

inline unsigned int generate_thread_seed(unsigned int seed, unsigned int thread_id) {
    constexpr unsigned int THREAD_SEED_OFFSET = 31;
    return seed + THREAD_SEED_OFFSET * thread_id;
}

void price_batch_worker(
    const model::EuropeanOption& option, 
    size_t batch_size,
    unsigned int random_seed,
    unsigned int thread_id,
    double& payoff_output
) {
    unsigned int thread_local_seed = generate_thread_seed(random_seed, thread_id);
    std::mt19937 gen(thread_local_seed);

    auto simulate_payoff = std::bind_front(calculate_discounted_payoff, std::ref(option), std::ref(gen));

    std::vector<double> payoffs(batch_size);
    std::ranges::generate(payoffs, simulate_payoff);
    
    double total_payoff = std::ranges::fold_left(payoffs, 0.0, std::plus{});
    payoff_output = total_payoff / static_cast<double>(batch_size);
}

#ifdef TEST_ALIGNED
struct alignas(64) aligned_double {
    double x;
};
#endif

MonteCarloPricer::MonteCarloPricer(
    unsigned int simulation_seed,
    const model::MontecarloParameters& simulation_parameters
) : simulation_seed_(simulation_seed), simulation_parameters_(simulation_parameters) {}

double MonteCarloPricer::price(
    const model::EuropeanOption& option
) const {
#ifdef TEST_ALIGNED
    std::vector<aligned_double> avg_payoffs(simulation_parameters_.thread_count); 
#else
    std::vector<double> avg_payoffs(simulation_parameters_.thread_count); 
#endif

    {
        std::size_t batch_size = simulation_parameters_.sample_count / simulation_parameters_.thread_count;

        std::vector<std::jthread> threads;
        threads.reserve(simulation_parameters_.thread_count);

        for (auto thread_id : std::views::iota(0u, simulation_parameters_.thread_count)) {
            threads.emplace_back(
                price_batch_worker, 
                std::ref(option), 
                batch_size,
                simulation_seed_,
                thread_id,
#ifdef TEST_ALIGNED
                std::ref(avg_payoffs[thread_id].x)
#else
                std::ref(avg_payoffs[thread_id])
#endif
            );
        }
    }

#ifdef TEST_ALIGNED
    double total_payoff = 0.0;
    for (std::size_t i = 0; i < simulation_parameters_.thread_count; ++i) {
        total_payoff += avg_payoffs[i].x;
    }
#else
    double total_payoff = std::ranges::fold_left(avg_payoffs, 0.0, std::plus{});
#endif

    return total_payoff / static_cast<double>(simulation_parameters_.thread_count);
}

}
}
