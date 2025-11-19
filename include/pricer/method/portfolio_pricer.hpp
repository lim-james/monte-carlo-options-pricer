#pragma once
#define BENCHMARK_OPTIONS

#include <print>
#include <chrono>
#include <vector>
#include <ranges>
#include <functional>

#include "pricer/model/european.h"
#include "pricer/model/perf_stats.h"
#include "pricer/model/option_greeks.h"
#include "pricer/model/priced_option.h"
#include "pricer/method/option_pricer.h"
#include "pricer/method/greeks/greeks.hpp"
#include "pricer/util/calc_perf_stats.h"

using hr_clock_t = std::chrono::high_resolution_clock;
using ms_t = std::chrono::duration<double, std::milli>;

namespace pricer {
namespace method::portfolio {

template<OptionPricer P>
std::vector<double> price_and_benchmark(
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

    model::PerfStats performance_stats = util::calculate_perf_stats(times);

    std::println("{:-^30}", " PRICING PERFORMANCE "); 
    std::println("Throughput: {:.03f} options/min", performance_stats.options_per_min);
    std::println("Mean time:  {:.03f} ±{:.03f}ms", performance_stats.mean_ms, performance_stats.std_ms);
    std::println("------------------------------");

    return payoffs;
}

template<OptionPricer P>
std::vector<double> price(
    const std::vector<model::EuropeanOption>& options, 
    const P& pricer
) {
    return options 
        | std::views::transform(std::bind_front(&P::price, pricer)) 
        | std::ranges::to<std::vector>();
}


template<OptionPricer P>
std::vector<model::OptionGreeks> calculate_greeks(
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
auto price_with_greeks(
    const std::vector<model::EuropeanOption>& options, 
    const P& pricer
) {
#ifdef BENCHMARK_OPTIONS
    auto payoffs = price_and_benchmark(options, pricer);
#else
    auto payoffs = price(options, pricer);
#endif

    auto greeks = calculate_greeks(options, pricer);

    auto priced_options = std::views::zip(options, payoffs, greeks) 
        | std::views::transform([](const auto& tuple) {
            const auto& [option, payoff, greeks] = tuple;
            return model::PricedOption{payoff, greeks, option};
        }) 
        | std::ranges::to<std::vector>();

    return priced_options;
}

}
}

