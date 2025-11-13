#ifndef PRICER_METHOD_MONTECARLO_H
#define PRICER_METHOD_MONTECARLO_H

#include "pricer/model/european.h"
#include "pricer/model/montecarlo_parameters.h"

namespace pricer {
namespace method::montecarlo {

double priceOption(
    const model::EuropeanOption& option, 
    const model::MontecarloParameters& params,
    const unsigned int simulation_seed
);

}
}

#endif
