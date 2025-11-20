#pragma once 

#include "pricer/model/perf_stats.h"

#include <vector>

namespace pricer {
namespace util {

model::PerfStats calculate_perf_stats(const std::vector<double>& times);

}
}
