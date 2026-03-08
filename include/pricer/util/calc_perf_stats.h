#pragma once 

#include "pricer/model/perf_stats.h"

#include <span>

namespace pricer {
namespace util {

model::PerfStats calculate_perf_stats(std::span<double> times);

}
}
