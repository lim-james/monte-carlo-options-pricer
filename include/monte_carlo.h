#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include "eu_option.h"
#include "monte_carlo_parameters.h"

struct alignas(64) aligned_double {
    double x;
};

double grow(const eu_option& option);
double discount(double future_price, double rate, double time_to_expiry);
double payoff_call(const eu_option& option);

void monte_carlo_call_pricing_batch(
    const eu_option& option, 
    size_t batch_size,
    aligned_double* payoff_mem
);
double monte_carlo_call_pricing(const eu_option& option, const monte_carlo_parameters& params);

#endif 
