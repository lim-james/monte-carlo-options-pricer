#include <print>
#include <random>
#include <thread>
#include <cassert>
#include <algorithm>
#include <filesystem>
#include <optional>

#include "pricer/model/montecarlo_parameters.h"
#include "pricer/method/blackscholes.h"
#include "pricer/method/montecarlo.h"
#include "pricer/method/portfolio_pricer.hpp"
#include "pricer/util/io.h"

struct CliArguments {
    std::filesystem::path filepath;
    std::size_t sample_count;
    std::size_t thread_count;
};

std::optional<CliArguments> parse_arguments(int argsc, const char* argsv[]) {
    if (argsc <= 1) {
        return std::nullopt;
    }

    constexpr std::size_t DEFAULT_SAMPLE_COUNT = 1'000'000;
    const std::size_t MAX_THREADS  = std::thread::hardware_concurrency();

    std::size_t sample_count = argsc > 2 ? std::stol(argsv[2]) : DEFAULT_SAMPLE_COUNT;
    std::size_t thread_count = argsc > 3 ? std::stoi(argsv[3]) : MAX_THREADS;

    return CliArguments{argsv[1], sample_count, thread_count}; 
}

int main(int argsc, const char* argsv[]) {
    auto arguments = parse_arguments(argsc, argsv);

    if (!arguments) {
        std::println("Please provide a .csv filepath");
        return 0;
    }

    auto european_options = pricer::util::load_options_from_csv(arguments->filepath);

    auto mc = pricer::method::montecarlo::make_monte_carlo_pricer(
        arguments->sample_count, 
        arguments->thread_count
    );
    pricer::method::blackscholes::BlackScholesPricer bs;

#ifdef BENCHMARK_OPTIONS
    auto [mc_priced_options, mc_stats] = pricer::method::portfolio::price_with_greeks(european_options, mc); 
    auto [bs_payoffs, bs_stats] = pricer::method::portfolio::price(european_options, bs); 
#else 
    auto mc_priced_options = pricer::method::portfolio::price_with_greeks(european_options, mc); 
    auto bs_payoffs = pricer::method::portfolio::price(european_options, bs); 
#endif

    assert(
        mc_priced_options.size() == bs_payoffs.size() &&
       "Mismatch in Monte Carlo and Black-Scholes payoff counts"
    );

    pricer::util::save_priced_options_to_csv("./output.csv", mc_priced_options);

    std::vector<double> diffs;
    diffs.reserve(bs_payoffs.size());

    auto mc_payoffs = mc_priced_options | std::views::transform([](auto priced_option){
        return priced_option.payoff;
    });
    for (const auto& [mc_payoff, bs_payoff] : std::views::zip(mc_payoffs, bs_payoffs)) 
        diffs.push_back(mc_payoff - bs_payoff);

    double abs_diff_mean = std::fabs(std::ranges::fold_left(diffs, 0.0, std::plus{}) / diffs.size());

    std::println("----- Performance Summary -----");
    std::println("Simulation Samples:  {}", arguments->sample_count);
    std::println("Requested threads:   {}", arguments->thread_count);
    std::println("Total options:       {}", european_options.size());
#ifdef BENCHMARK_OPTIONS
    std::println("MC Throughput:       {:.03f} options/min", mc_stats.options_per_min);
    std::println("BS Throughput:       {:.03f} options/min", bs_stats.options_per_min);
    std::println("MC mean time:        {:.03f} ±{:.03f}ms", mc_stats.mean_ms, mc_stats.std_ms);
    std::println("BS mean time:        {:.03f} ±{:.03f}ms", bs_stats.mean_ms, bs_stats.std_ms);
#endif
    std::println("Mean |MC-BS|:        {:.03f}", abs_diff_mean);
    std::println("-------------------------------");

    return 0;
}
