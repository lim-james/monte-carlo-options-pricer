#include "pricer/util/calc_perf_stats.h"

#include <cmath>
#include <ranges>
#include <algorithm>

namespace pricer {
namespace util {

inline double standard_deviation(std::span<double> values, double mean) {
    auto squared_diff = [mean](double x){return (x - mean) * (x - mean);};
    const double variance_sum = std::ranges::fold_left(
        values | std::views::transform(squared_diff),
        0.0,
        std::plus{}
    );
    return std::sqrt(variance_sum / values.size());
}

model::PerfStats calculate_perf_stats(std::span<double> times) {
    const std::size_t num_options = times.size();
    model::PerfStats stats;
    stats.total_ms = std::ranges::fold_left(times, 0.0, std::plus{});
    stats.mean_ms  = stats.total_ms / num_options;
    stats.std_ms   = standard_deviation(times, stats.mean_ms);

    constexpr double MS_PER_MINUTES = 60'000.0;
    double total_mins     = stats.total_ms / MS_PER_MINUTES;
    stats.options_per_min = num_options / total_mins;
    return stats;
}


}
}
