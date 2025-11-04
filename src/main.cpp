#include <print>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <numeric>

#include "eu_option_out.h"

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

inline double eval_finite_step(double value) {
    return 0.01 * value;
}

template<typename PricingFn>
inline double eval_finite_difference_first(
    const eu_option& up, 
    const eu_option& dn,
    double h,
    PricingFn&& pricing_fn
) {
    const double up_payoff = pricing_fn(up);   
    const double dn_payoff = pricing_fn(dn);   
    return (up_payoff - dn_payoff) / (2 * h);
}

template<typename PricingFn>
double eval_delta(
    const eu_option& option, 
    PricingFn&& pricing_fn
) {
    const double h = eval_finite_step(option.spot);
    eu_option option_up = option;
    option_up.spot += h;

    eu_option option_dn = option;
    option_dn.spot -= h;

    return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
}

template<typename PricingFn>
double eval_gamma(
    const eu_option& option, 
    PricingFn&& pricing_fn
) {
    const double h = eval_finite_step(option.spot);
    eu_option option_up = option;
    option_up.spot += h;

    eu_option option_dn = option;
    option_dn.spot -= h;

    const double payoff = pricing_fn(option);
    const double up_payoff = pricing_fn(option_up);   
    const double dn_payoff = pricing_fn(option_dn);   
    return (up_payoff - 2 * payoff + dn_payoff) / (h * h);
}

template<typename PricingFn>
double eval_vega(
    const eu_option& option, 
    PricingFn&& pricing_fn
) {
    const double h = eval_finite_step(option.volatility);
    eu_option option_up = option;
    option_up.volatility += h;

    eu_option option_dn = option;
    option_dn.volatility -= h;

    return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
}

template<typename PricingFn>
double eval_theta(
    const eu_option& option, 
    PricingFn&& pricing_fn
) {
    const double h = eval_finite_step(option.expiry);
    eu_option option_up = option;
    option_up.expiry += h;

    eu_option option_dn = option;
    option_dn.expiry -= h;

    return -eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
}

template<typename PricingFn>
double eval_rho(
    const eu_option& option, 
    PricingFn&& pricing_fn
) {
    const double h = eval_finite_step(option.rate);
    eu_option option_up = option;
    option_up.rate += h;

    eu_option option_dn = option;
    option_dn.rate -= h;

    return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
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
    
    std::vector<eu_option_out> priced_options;
    priced_options.reserve(num_options);

    std::vector<double> mc_times, bs_times, diffs;
    mc_times.reserve(num_options);
    bs_times.reserve(num_options);
    diffs.reserve(num_options);

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

        double delta = eval_delta(option, mc_pricing);
        double gamma = eval_gamma(option, mc_pricing);
        double vega = eval_vega(option, mc_pricing);
        double theta = eval_theta(option, mc_pricing);
        double rho = eval_rho(option, mc_pricing);

        start = hr_clock_t::now();
        const double bs_payoff = black_scholes_pricing(option);
        end = hr_clock_t::now();

        auto bs_dt = ms_t(end - start).count();

        // std::println("delta BS: {}", eval_delta(option, black_scholes_pricing));

        priced_options.emplace_back(eu_option_out{
            option.type,
            option.spot,
            option.strike,
            option.expiry,
            option.volatility,
            option.rate,
            mc_payoff,
            delta,
            gamma,
            vega,
            rho,
            theta
        });
        mc_times.emplace_back(mc_dt);
        bs_times.emplace_back(bs_dt);
        diffs.emplace_back(mc_payoff - bs_payoff);
    }

    save_options_to_csv("./output.csv", priced_options);

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
