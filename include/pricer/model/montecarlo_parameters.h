#pragma once

#include <cstddef>

namespace pricer::model {

struct MontecarloParameters {
    std::size_t sample_count; 
    std::size_t thread_count;
};

}
