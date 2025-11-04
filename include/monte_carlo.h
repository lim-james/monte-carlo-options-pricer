#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include "eu_option.h"
#include "monte_carlo_parameters.h"

#include <random>

double monte_carlo_pricing(
    const eu_option& option, 
    const monte_carlo_parameters& params,
    const unsigned int seed
);

#endif 
