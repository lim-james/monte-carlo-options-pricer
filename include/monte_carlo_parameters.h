#ifndef MONTE_CARLO_PARAMETERS_H
#define MONTE_CARLO_PARAMETERS_H

#include <cstddef>
#include <cstdint>

struct monte_carlo_parameters {
    size_t sample_count; 
    uint32_t thread_count;
};

#endif
