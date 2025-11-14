#include "pricer/util/calc_perf_stats.h"

#include <numeric>
#include <cmath>

namespace pricer {
namespace util {

inline double stdev(const std::vector<double>& sample, double mean) {
    double acc = 0.0;
    for (auto x: sample) acc += (x - mean) * (x - mean);
    return std::sqrt(acc / sample.size());
}

model::PerfStats calculatePerfStats(const std::vector<double>& times) {
    size_t num_options = times.size();
    model::PerfStats st;
    st.total_ms        = std::accumulate(times.begin(), times.end(), 0.0);
    st.mean_ms         = st.total_ms / num_options;
    st.std_ms          = stdev(times, st.mean_ms);
    st.options_per_min = num_options / (st.total_ms / 1000.0 / 60.0);
    return st;
}


}
}
