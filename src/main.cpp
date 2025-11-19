#include <print>
#include <cassert>
#include <ranges>

#include "pricer/cli/parse_arguments.h"
#include "pricer/method/blackscholes.h"
#include "pricer/method/montecarlo.h"
#include "pricer/method/portfolio_pricer.hpp"
#include "pricer/util/io.h"
#include "pricer/util/mean_absolute_difference.hpp"


int main(int argsc, const char* argsv[]) {
    auto arguments = parser::cli::parse_arguments(argsc, argsv);

    if (!arguments) {
        std::println("Please provide a .csv filepath");
        return 0;
    }

    auto european_options = pricer::util::load_options_from_csv(arguments->filepath);

    std::println("{:-^30}", " SIMULATION DETAILS "); 
    std::println("Simulation samples: {}", arguments->sample_count);
    std::println("Requested threads:  {}", arguments->thread_count);
    std::println("Total options:      {}", european_options.size());
    std::println("------------------------------");

    auto mc = pricer::method::montecarlo::make_monte_carlo_pricer(
        arguments->sample_count, 
        arguments->thread_count
    );
    auto bs = pricer::method::blackscholes::BlackScholesPricer{};

    auto mc_priced_options = pricer::method::portfolio::price_with_greeks(european_options, mc); 
    auto bs_payoffs = pricer::method::portfolio::price(european_options, bs); 

    constexpr std::string_view OUTPUT_CSV = "./output.csv";
    pricer::util::save_priced_options_to_csv(OUTPUT_CSV, mc_priced_options);

    auto mc_payoffs = mc_priced_options | std::views::transform(&pricer::model::PricedOption::payoff);
    double abs_diff_mean = pricer::util::mean_absolute_difference(mc_payoffs, bs_payoffs);
    std::println("{:-^30}", " VALIDATION "); 
    std::println("Mean |MC-BS|:       {:.03f}", abs_diff_mean);
    std::println("------------------------------");

    return 0;
}
