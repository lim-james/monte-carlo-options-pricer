#include <print>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <numeric>

#include "greeks.hpp"
#include "option_greeks.h"

#include "rapidcsv.h"
#include "monte_carlo.h"
#include "black_scholes.h"
#include "io.h"

using hr_clock_t = std::chrono::high_resolution_clock;
using ms_t = std::chrono::duration<double, std::milli>;

struct perf_stats {
    double mc_ms_total     = 0.0;
    double bs_ms_total     = 0.0;
    double mc_mean_ms      = 0.0;
    double bs_mean_ms      = 0.0;
    double mc_std_ms       = 0.0;
    double bs_std_ms       = 0.0;
    double options_per_min = 0.0;
    double abs_diff_mean   = 0.0;
};

double stdev(const std::vector<double>& sample, double mean) {
    double acc = 0.0;
    for (auto x: sample) acc += (x - mean) * (x - mean);
    return std::sqrt(acc / sample.size());
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

    const size_t num_options = eu_options_list.size();
    double total_time = 0.0;
    
    std::vector<double> mc_payoffs, mc_times;
    mc_payoffs.reserve(num_options);
    mc_times.reserve(num_options);

    std::vector<option_greeks> greeks;
    greeks.reserve(num_options);

    std::random_device rd;
    for (const eu_option& option: eu_options_list) {
        const unsigned int seed = rd();
        const auto mc_pricing = [&params, seed](const eu_option& op) {
            return monte_carlo_pricing(op, params, seed); 
        };

        auto start = hr_clock_t::now();
        const double mc_payoff = mc_pricing(option);
        auto end = hr_clock_t::now();
        auto mc_dt = ms_t(end - start).count();

        greeks.push_back(greeks::calculate(option, mc_pricing));

        mc_times.push_back(mc_dt);
        mc_payoffs.push_back(mc_payoff);
    }

    std::vector<double> bs_payoffs, bs_times;
    bs_payoffs.reserve(num_options);
    bs_times.reserve(num_options);
    for (const eu_option& option: eu_options_list) {
        auto start = hr_clock_t::now();
        const double bs_payoff = black_scholes_pricing(option);
        auto end = hr_clock_t::now();

        auto bs_dt = ms_t(end - start).count();
        bs_times.push_back(bs_dt);
        bs_payoffs.push_back(bs_payoff);
    }

    assert(
        mc_payoffs.size() == bs_payoffs.size() &&
       "Mismatch in Monte Carlo and Black-Scholes payoff counts"
    );

    std::vector<double> diffs;
    diffs.reserve(num_options);
    for (size_t i = 0; i < mc_payoffs.size(); ++i) {
        diffs.push_back(mc_payoffs[i] - bs_payoffs[i]);
    }

    save_options_to_csv("./output.csv", eu_options_list, mc_payoffs, greeks);

    perf_stats st;
    st.mc_ms_total     = std::accumulate(mc_times.begin(), mc_times.end(), 0.0);
    st.bs_ms_total     = std::accumulate(bs_times.begin(), bs_times.end(), 0.0);
    st.mc_mean_ms      = st.mc_ms_total / num_options;
    st.bs_mean_ms      = st.bs_ms_total / num_options;
    st.mc_std_ms       = stdev(mc_times, st.mc_mean_ms);
    st.bs_std_ms       = stdev(bs_times, st.bs_mean_ms);
    st.options_per_min = num_options / (st.mc_ms_total / 1000.0 / 60.0);
    st.abs_diff_mean   = std::fabs(std::accumulate(diffs.begin(), diffs.end(), 0.0) / diffs.size());


    std::println("----- Performance Summary -----");
    std::println("Simulation Samples:  {}", params.sample_count);
    std::println("Requested threads:   {}", params.thread_count);
    std::println("Total options:       {}", eu_options_list.size());
    // std::println("Total runtime:       {} s", total_ms / 1000.0);
    std::println("Throughput:          {:.03f} options/min", st.options_per_min);
    std::println("MC mean time:        {:.03f} ±{:.03f}ms", st.mc_mean_ms, st.mc_std_ms);
    std::println("BS mean time:        {:.03f} ±{:.03f}ms", st.bs_mean_ms, st.bs_std_ms);
    std::println("Mean |MC-BS|:        {:.03f}", st.abs_diff_mean);
    std::println("-------------------------------");

    return 0;
}
