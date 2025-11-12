#include <print>
#include <random>
#include <cmath>
#include <chrono>
#include <string>
#include <thread>
#include <numeric>
#include <ranges>
#include <tuple>

#include "greeks.hpp"
#include "option_greeks.h"

#include "rapidcsv.h"
#include "eu_option.h"
#include "monte_carlo.h"
#include "black_scholes.h"
#include "io.h"

using hr_clock_t = std::chrono::high_resolution_clock;
using ms_t = std::chrono::duration<double, std::milli>;

struct perf_stats {
    double total_ms        = 0.0;
    double mean_ms         = 0.0;
    double std_ms          = 0.0;
    double options_per_min = 0.0;
};

struct simulation_response {
    std::vector<double> payoffs;
    std::vector<option_greeks> greeks;
};

typedef std::tuple<simulation_response, perf_stats> response_type ; 

double stdev(const std::vector<double>& sample, double mean) {
    double acc = 0.0;
    for (auto x: sample) acc += (x - mean) * (x - mean);
    return std::sqrt(acc / sample.size());
}

perf_stats calc_perf_stats(const std::vector<double>& times) {
    size_t num_options = times.size();
    perf_stats st;
    st.total_ms        = std::accumulate(times.begin(), times.end(), 0.0);
    st.mean_ms         = st.total_ms / num_options;
    st.std_ms          = stdev(times, st.mean_ms);
    st.options_per_min = num_options / (st.total_ms / 1000.0 / 60.0);
    return st;
}

response_type monte_carlo(
    const std::vector<eu_option>& options, 
    const monte_carlo_parameters& params
) {
    size_t num_options = options.size();

    simulation_response response;
    response.payoffs.reserve(num_options);
    response.greeks.reserve(num_options);

    std::vector<double> times;
    times.reserve(num_options);

    std::random_device rd;
    for (const eu_option& option: options) {
        unsigned int seed = rd();
        auto pricing = [&params, seed](const eu_option& op) {
            return monte_carlo_pricing(op, params, seed); 
        };

        auto start = hr_clock_t::now();
        const double payoff = pricing(option);
        auto end = hr_clock_t::now();

        auto dt = ms_t(end - start).count();
        times.push_back(dt);

        response.payoffs.push_back(payoff);
        response.greeks.push_back(greeks::calculate(option, pricing));
    }

    perf_stats st = calc_perf_stats(times);

    return {response, st};
}

response_type black_scholes(const std::vector<eu_option>& options) {
    size_t num_options = options.size();

    simulation_response response;
    response.payoffs.reserve(num_options);

    std::vector<double> times;
    times.reserve(num_options);

    for (const eu_option& option: options) {
        auto start = hr_clock_t::now();
        double payoff = black_scholes_pricing(option);
        auto end = hr_clock_t::now();

        auto dt = ms_t(end - start).count();
        times.push_back(dt);

        response.payoffs.push_back(payoff);
    }

    perf_stats st = calc_perf_stats(times);

    return {response, st};
}

int main(int argsc, const char* argsv[]) {
    if (argsc <= 1) {
        std::println("Please provide a .csv filepath");
        return 0;

    }
    auto eu_options_list = load_options_from_csv(argsv[1]);

    const uint32_t sample_count = argsc > 2 ? std::stol(argsv[2]) : 1000000;
    const uint32_t max_threads  = std::thread::hardware_concurrency();
    const uint32_t thread_count = argsc > 3 ? std::stoi(argsv[3]) : max_threads;
    const monte_carlo_parameters params{sample_count, thread_count};

    auto [mc, mc_st] = monte_carlo(eu_options_list, params);
    auto [bs, bs_st] = black_scholes(eu_options_list);

    assert(
        mc.payoffs.size() == bs.payoffs.size() &&
       "Mismatch in Monte Carlo and Black-Scholes payoff counts"
    );

    std::vector<double> diffs;
    diffs.reserve(mc.payoffs.size());

    for (const auto& [mc_payoff, bs_payoff] : std::views::zip(mc.payoffs, bs.payoffs)) 
        diffs.push_back(mc_payoff - bs_payoff);

    save_options_to_csv("./output.csv", eu_options_list, mc.payoffs, mc.greeks);

    double abs_diff_mean = std::fabs(std::accumulate(diffs.begin(), diffs.end(), 0.0) / diffs.size());

    std::println("----- Performance Summary -----");
    std::println("Simulation Samples:  {}", params.sample_count);
    std::println("Requested threads:   {}", params.thread_count);
    std::println("Total options:       {}", eu_options_list.size());
    std::println("MC Throughput:       {:.03f} options/min", mc_st.options_per_min);
    std::println("BS Throughput:       {:.03f} options/min", bs_st.options_per_min);
    std::println("MC mean time:        {:.03f} ±{:.03f}ms", mc_st.mean_ms, mc_st.std_ms);
    std::println("BS mean time:        {:.03f} ±{:.03f}ms", bs_st.mean_ms, bs_st.std_ms);
    std::println("Mean |MC-BS|:        {:.03f}", abs_diff_mean);
    std::println("-------------------------------");

    return 0;
}
