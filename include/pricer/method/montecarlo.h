#pragma once

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

public:
    MonteCarloPricer(
        unsigned int simulation_seed,
        const model::MontecarloParameters& params
    );

    double price(const model::EuropeanOption& option) const;

};

template<typename ...Args>
MonteCarloPricer makeMonteCarloPricer(Args&&... args) {
    thread_local std::random_device rd;
    return MonteCarloPricer{
        rd(), 
        model::MontecarloParameters{std::forward<Args>(args)...}
    };
}

}
}

