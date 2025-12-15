#pragma once

#include "pricer/concurrency/work_queue.h"
#include "pricer/model/european.h"
#include "pricer/model/montecarlo_parameters.h"

#include <random>
#include <utility>

namespace pricer {
namespace method::montecarlo {

class MonteCarloPricer {
private:
    const unsigned int simulation_seed_; 
    const model::MontecarloParameters simulation_parameters_;

    struct alignas(64) PaddedDouble {
        double x;
    };

public:
    MonteCarloPricer(
        unsigned int simulation_seed,
        const model::MontecarloParameters& params
    );

    double price(const model::EuropeanOption& option) const;

private:

    void breakup_workload(
        WorkQueue& queue, 
        std::size_t sample_count
    ) const;

    void start_workers(
        const model::EuropeanOption& option,
        WorkQueue& queue, 
        std::vector<PaddedDouble>& payoffs
    ) const;

};

template<typename ...Args>
MonteCarloPricer make_monte_carlo_pricer(Args&&... args) {
    thread_local std::random_device rd;
    return MonteCarloPricer{
        rd(), 
        model::MontecarloParameters{std::forward<Args>(args)...}
    };
}

}
}

