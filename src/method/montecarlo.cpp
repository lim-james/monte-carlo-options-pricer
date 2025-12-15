#include "pricer/method/montecarlo.h"
#include "pricer/method/montecarlo_worker.h"
#include "pricer/concurrency/work_queue.h"

#include <functional>
#include <algorithm>
#include <ranges>

namespace pricer {
namespace method::montecarlo {

MonteCarloPricer::MonteCarloPricer(
    unsigned int simulation_seed,
    const model::MontecarloParameters& simulation_parameters
) 
    : simulation_seed_(simulation_seed)
    , simulation_parameters_(simulation_parameters) {}


double MonteCarloPricer::price(
    const model::EuropeanOption& option
) const {
    std::vector<PaddedDouble> avg_payoffs(simulation_parameters_.thread_count); 

    WorkQueue queue;
    breakup_workload(queue, simulation_parameters_.sample_count);
    start_workers(option, queue, avg_payoffs);

    double total_payoff = std::ranges::fold_left(
        avg_payoffs | std::views::transform([](auto x) { return x.x; }),
        0.0, std::plus{}
    );

    return total_payoff / static_cast<double>(simulation_parameters_.sample_count);
}

void MonteCarloPricer::breakup_workload(
    WorkQueue& queue, 
    std::size_t sample_count
) const {
    constexpr std::size_t BATCH = 1'000;
    while (sample_count > BATCH) {
        queue.push(BATCH);
        sample_count -= BATCH;
    }

    queue.push(std::min(sample_count, BATCH));
}

void MonteCarloPricer::start_workers(
    const model::EuropeanOption& option,
    WorkQueue& queue, 
    std::vector<PaddedDouble>& payoffs
) const {
    std::vector<MCWorker> workers;
    workers.reserve(simulation_parameters_.thread_count);

    for (std::size_t thread_id{}; thread_id < simulation_parameters_.thread_count; ++thread_id) {
        workers.emplace_back(
            option,
            simulation_seed_,
            thread_id,
            &payoffs[thread_id].x,
            std::ref(queue)
        );
    }
}

}
}
