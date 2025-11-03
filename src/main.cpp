#include <print>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <numeric>

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
    const uint32_t thread_count = argsc > 3 ? std::stoi(argsv[3]) : std::thread::hardware_concurrency();
    const monte_carlo_parameters params{sample_count, thread_count};

    const size_t num_options = eu_options_list.size();
    double total_time = 0.0;
    
    std::vector<double> payoffs;
    payoffs.reserve(num_options);

    std::vector<double> mc_times, bs_times, diffs;
    mc_times.reserve(num_options);
    bs_times.reserve(num_options);
    diffs.reserve(num_options);

    for (const eu_option& option: eu_options_list) {
        auto start = hr_clock_t::now();
        const double mc_payoff = monte_carlo_pricing(option, params);
        auto end = hr_clock_t::now();
        
        auto mc_dt = ms_t(end - start).count();

        start = hr_clock_t::now();
        const double bs_payoff = black_scholes_pricing(option);
        end = hr_clock_t::now();

        auto bs_dt = ms_t(end - start).count();

        payoffs.emplace_back(mc_payoff);
        mc_times.emplace_back(mc_dt);
        bs_times.emplace_back(bs_dt);
        diffs.emplace_back(mc_payoff - bs_payoff);
    }

    save_payoffs_to_csv("./output.csv", payoffs);

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
