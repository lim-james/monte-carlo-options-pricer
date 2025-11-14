#pragma once
// #define BENCHMARK_OPTIONS

#include <chrono>
#include <vector>
#include <ranges>
#include <functional>

#include "pricer/model/european.h"
#include "pricer/model/perf_stats.h"
#include "pricer/model/option_greeks.h"
#include "pricer/method/option_pricer.h"
#include "pricer/method/greeks/greeks.hpp"
#include "pricer/util/calc_perf_stats.h"

using hr_clock_t = std::chrono::high_resolution_clock;
using ms_t = std::chrono::duration<double, std::milli>;

namespace pricer {
namespace method::portfolio {

#ifdef BENCHMARK_OPTIONS

template<OptionPricer P>
std::tuple<std::vector<double>, model::PerfStats> price(
    const std::vector<model::EuropeanOption>& options, 
    const P& method
) {
    std::vector<double> payoffs, times;
    payoffs.reserve(options.size());
    times.reserve(options.size());


    for (const auto& opt: options) {
        auto start = hr_clock_t::now();
        double payoff = method.price(opt);
        auto end = hr_clock_t::now();
        auto dt = ms_t(end - start).count();

        times.push_back(dt);
        payoffs.push_back(payoff);
    }

    return {payoffs, util::calculatePerfStats(times)};
}

#else

template<OptionPricer P>
std::vector<double> price(
    const std::vector<model::EuropeanOption>& options, 
    const P& method
) {
    return options 
    | std::views::transform(std::bind_front(&P::price, method)) 
    | std::ranges::to<std::vector>();
}

#endif

template<OptionPricer P>
std::vector<model::OptionGreeks> calculateGreeks(
    const std::vector<model::EuropeanOption>& options, 
    const P& method
) {
    auto greek_lambda = [&method](auto opt) {
        return greeks::calculate(opt, std::bind_front(&P::price, method));
    };

    return options 
    | std::views::transform(greek_lambda) 
    | std::ranges::to<std::vector>();
}

}
}
