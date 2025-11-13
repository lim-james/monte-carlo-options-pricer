#ifndef PRICER_MODEL_MONTECARLO_PARAMETERS_H
#define PRICER_MODEL_MONTECARLO_PARAMETERS_H

#include <cstddef>
#include <cstdint>

namespace pricer::model {

struct MontecarloParameters {
    size_t sample_count; 
    uint32_t thread_count;
};

}

#endif
