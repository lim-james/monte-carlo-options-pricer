#include <print>
#include <random>
#include <thread>
#include <cassert>

#include "pricer/model/montecarlo_parameters.h"
#include "pricer/method/blackscholes.h"
#include "pricer/method/montecarlo.h"
#include "pricer/method/portfolio_pricer.h"
#include "pricer/util/io.h"

int main(int argsc, const char* argsv[]) {
    if (argsc <= 1) {
        std::println("Please provide a .csv filepath");
        return 0;

    }
    auto eu_options_list = pricer::util::loadOptionsFromCsv(argsv[1]);

    uint32_t sample_count = argsc > 2 ? std::stol(argsv[2]) : 1000000;
    uint32_t max_threads  = std::thread::hardware_concurrency();
    uint32_t thread_count = argsc > 3 ? std::stoi(argsv[3]) : max_threads;
    pricer::model::MontecarloParameters params{sample_count, thread_count};

    std::random_device rd;
    unsigned int seed = rd();
    pricer::method::montecarlo::MonteCarloPricer mc{seed, params};

    pricer::method::blackscholes::BlackScholesPricer bs;

#ifdef BENCHMARK_OPTIONS
    auto [mc_payoffs, mc_st] = pricer::method::portfolio::price(eu_options_list, mc); 
    auto [bs_payoffs, bs_st] = pricer::method::portfolio::price(eu_options_list, bs); 
#else 
    auto mc_payoffs = pricer::method::portfolio::price(eu_options_list, mc); 
    auto bs_payoffs = pricer::method::portfolio::price(eu_options_list, bs); 
#endif

    auto mc_greeks  = pricer::method::portfolio::calculateGreeks(eu_options_list, mc); 

    assert(
        mc_payoffs.size() == bs_payoffs.size() &&
       "Mismatch in Monte Carlo and Black-Scholes payoff counts"
    );

    pricer::util::savePricedOptionsToCsv(
        "./output.csv", 
        eu_options_list, 
        mc_payoffs, 
        mc_greeks
    );

    std::vector<double> diffs;
    diffs.reserve(mc_payoffs.size());

    for (const auto& [mc_payoff, bs_payoff] : std::views::zip(mc_payoffs, bs_payoffs)) 
        diffs.push_back(mc_payoff - bs_payoff);

    double abs_diff_mean = std::fabs(std::accumulate(diffs.begin(), diffs.end(), 0.0) / diffs.size());

    std::println("----- Performance Summary -----");
    std::println("Simulation Samples:  {}", params.sample_count);
    std::println("Requested threads:   {}", params.thread_count);
    std::println("Total options:       {}", eu_options_list.size());
#ifdef BENCHMARK_OPTIONS
    std::println("MC Throughput:       {:.03f} options/min", mc_st.options_per_min);
    std::println("BS Throughput:       {:.03f} options/min", bs_st.options_per_min);
    std::println("MC mean time:        {:.03f} ±{:.03f}ms", mc_st.mean_ms, mc_st.std_ms);
    std::println("BS mean time:        {:.03f} ±{:.03f}ms", bs_st.mean_ms, bs_st.std_ms);
#endif
    std::println("Mean |MC-BS|:        {:.03f}", abs_diff_mean);
    std::println("-------------------------------");

    return 0;
}
