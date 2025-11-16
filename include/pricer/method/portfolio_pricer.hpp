#pragma once
#include "pricer/model/priced_option.h"
#define BENCHMARK_OPTIONS

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
    const P& pricer
) {
    std::vector<double> payoffs, times;
    payoffs.reserve(options.size());
    times.reserve(options.size());


    for (const auto& opt: options) {
        auto start = hr_clock_t::now();
        double payoff = pricer.price(opt);
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
    const P& pricer
) {
    return options 
    | std::views::transform(std::bind_front(&P::price, pricer)) 
    | std::ranges::to<std::vector>();
}

#endif

template<OptionPricer P>
std::vector<model::OptionGreeks> calculateGreeks(
    const std::vector<model::EuropeanOption>& options, 
    const P& pricer
) {
    auto bound_pricer = std::bind_front(&P::price, std::ref(pricer));
    auto calculate_greek = [&bound_pricer](auto opt) {
        return greeks::calculate(opt, bound_pricer);
    };

    return options 
        | std::views::transform(calculate_greek) 
        | std::ranges::to<std::vector>();
}

template<OptionPricer P>
auto priceWithGreeks(
    const std::vector<model::EuropeanOption>& options, 
    const P& pricer
) {
#ifdef BENCHMARK_OPTIONS
    auto [payoffs, stats] = price(options, pricer);
#else
    auto payoffs = price(options, pricer);
#endif

    auto greeks = calculateGreeks(options, pricer);

    auto priced_options = std::views::zip(options, payoffs, greeks) 
        | std::views::transform([](const auto& tuple) {
            const auto& [option, payoff, greeks] = tuple;
            return model::PricedOption{payoff, greeks, option};
        }) 
        | std::ranges::to<std::vector>();

#ifdef BENCHMARK_OPTIONS
    return std::pair{priced_options, stats};
#else
    return priced_options;
#endif
}

}
}

