#pragma once

#include <cstddef>
#include <cstdint>

namespace pricer::model {

struct MontecarloParameters {
    size_t sample_count; 
    uint32_t thread_count;
};

}
