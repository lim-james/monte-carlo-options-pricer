#include <print>
#include <random>
#include <thread>

#include "pricer/model/montecarlo_parameters.h"
#include "pricer/method/blackscholes.h"
#include "pricer/method/montecarlo.h"
#include "pricer/util/io.h"

using hr_clock_t = std::chrono::high_resolution_clock;
using ms_t = std::chrono::duration<double, std::milli>;

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

    pricer::method::blackscholes::priceOption(eu_options_list[0]);

    std::random_device rd;
    unsigned int seed = rd();
    pricer::method::montecarlo::priceOption(eu_options_list[0], params, seed);


    // auto [bs, bs_st] = pricer::method::blackscholes::priceOption(eu_options_list);

    // assert(
    //     mc.payoffs.size() == bs.payoffs.size() &&
    //    "Mismatch in Monte Carlo and Black-Scholes payoff counts"
    // );

    // std::vector<double> diffs;
    // diffs.reserve(mc.payoffs.size());

    // for (const auto& [mc_payoff, bs_payoff] : std::views::zip(mc.payoffs, bs.payoffs)) 
    //     diffs.push_back(mc_payoff - bs_payoff);

    // pricer::util::saveOptionsToCsv("./output.csv", eu_options_list, mc.payoffs, mc.greeks);

    //  double abs_diff_mean = std::fabs(std::accumulate(diffs.begin(), diffs.end(), 0.0) / diffs.size());

    // std::println("----- Performance Summary -----");
    // std::println("Simulation Samples:  {}", params.sample_count);
    // std::println("Requested threads:   {}", params.thread_count);
    // std::println("Total options:       {}", eu_options_list.size());
    // std::println("MC Throughput:       {:.03f} options/min", mc_st.options_per_min);
    // std::println("BS Throughput:       {:.03f} options/min", bs_st.options_per_min);
    // std::println("MC mean time:        {:.03f} ±{:.03f}ms", mc_st.mean_ms, mc_st.std_ms);
    // std::println("BS mean time:        {:.03f} ±{:.03f}ms", bs_st.mean_ms, bs_st.std_ms);
    // std::println("Mean |MC-BS|:        {:.03f}", abs_diff_mean);
    // std::println("-------------------------------");

    return 0;
}
