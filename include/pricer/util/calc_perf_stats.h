#pragma once 

#include "pricer/model/perf_stats.h"

#include <vector>

namespace pricer {
namespace util {

model::PerfStats calculatePerfStats(const std::vector<double>& times);

}
}
