#pragma once

#include "pricer/model/european.h"
#include "pricer/model/montecarlo_parameters.h"

namespace pricer {
namespace method::montecarlo {

class MonteCarloPricer {
private:
    const unsigned int simulation_seed_; 
    const model::MontecarloParameters simulation_parameters_;

public:
    MonteCarloPricer(
        const unsigned int simulation_seed,
        const model::MontecarloParameters& params
    );

    double price(const model::EuropeanOption& option) const;
};

}
}

