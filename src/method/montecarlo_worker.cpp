#include "pricer/method/montecarlo_worker.h"
#include "pricer/concurrency/work_queue.h"

#include <functional>

namespace pricer {

MCWorker::MCWorker(
    const model::EuropeanOption& option,
    unsigned int random_seed,
    unsigned int thread_id,
    double* payoff_output,
    WorkQueue& queue
) 
    : option_(option)
    , gen_(generate_thread_seed(random_seed, thread_id))
    , payoff_output_(payoff_output) 
    , thread_(std::bind_front(&MCWorker::poll, this), std::ref(queue)){}

inline unsigned int MCWorker::generate_thread_seed(unsigned int seed, unsigned int thread_id) {
    constexpr unsigned int THREAD_SEED_OFFSET = 31;
    return seed + THREAD_SEED_OFFSET * thread_id;
}

inline double MCWorker::apply_discount(double future_price, double market_rate, double time_to_expiry) {
    return future_price * std::exp(-market_rate * time_to_expiry);
}

double MCWorker::calculate_future_price() {
    thread_local std::normal_distribution<> dist(0.0, 1.0);

    // TODO: figure out better names
    double Z = dist(gen_);
    double a = option_.risk_free_rate - 0.5 * option_.implied_volatility * option_.implied_volatility;
    double b = option_.implied_volatility * std::sqrt(option_.time_to_expiry) * Z;
    double c = std::exp(a * option_.time_to_expiry + b);

    return option_.spot * c;
}

double MCWorker::calculate_discounted_payoff() {
    double future_price = calculate_future_price();
    double raw_payoff = option_.type == model::OptionType::Call 
        ? std::max(future_price - option_.strike, 0.0)
        : std::max(option_.strike - future_price, 0.0);

    double discounted_payoff = apply_discount(
        raw_payoff, 
        option_.risk_free_rate, 
        option_.time_to_expiry
    );

    return discounted_payoff;
}

void MCWorker::poll(WorkQueue& queue) {
    double total_payoff{};
    std::size_t batch_size;
    while (queue.pop(batch_size)) {
        for (std::size_t i{}; i < batch_size; ++i)
            total_payoff += calculate_discounted_payoff();
    }

    *payoff_output_ += total_payoff;
}

}
