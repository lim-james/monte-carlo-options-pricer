#pragma once

#include <tuple>

#include "pricer/model/european.h"

namespace pricer {
namespace method::greeks::internal {
    inline double calculate_finite_step(double value) {
        return 0.01 * value;
    }

    inline double calculate_finite_difference_first_order(
        const model::EuropeanOption& option_upper, 
        const model::EuropeanOption& option_lower,
        double h,
        auto&& pricer
    ) {
        double upper_payoff = pricer(option_upper);   
        double lower_payoff = pricer(option_lower);   
        return (upper_payoff - lower_payoff) / (2.0 * h);
    }

    inline double calculate_finite_difference_second_order(
        const model::EuropeanOption& option, 
        const model::EuropeanOption& option_upper, 
        const model::EuropeanOption& option_lower,
        double h,
        auto&& pricer
    ) {
        double payoff = pricer(option);
        double upper_payoff = pricer(option_upper);   
        double lower_payoff = pricer(option_lower);   
        return (upper_payoff - 2.0 * payoff + lower_payoff) / (h * h);
    }

    template<typename MemberPtr> 
    auto create_perturbed_options(const model::EuropeanOption& option, MemberPtr member_ptr) {
        double h = calculate_finite_step(option.*member_ptr);

        model::EuropeanOption option_upper = option;
        option_upper.*member_ptr += h;
        model::EuropeanOption option_lower = option;
        option_lower.*member_ptr -= h;

        return std::tuple{h, option_upper, option_lower};
    }

    double delta(const model::EuropeanOption& option, auto&& pricer) {
        auto [h, option_upper, option_lower] = create_perturbed_options(option, &model::EuropeanOption::spot);
        return calculate_finite_difference_first_order(option_upper, option_lower, h, pricer);
    }

    double gamma(const model::EuropeanOption& option, auto&& pricer) {
        auto [h, option_upper, option_lower] = create_perturbed_options(option, &model::EuropeanOption::spot);
        return calculate_finite_difference_second_order(option, option_upper, option_lower, h, pricer);
    }

    double vega(const model::EuropeanOption& option, auto&& pricer) {
        auto [h, option_upper, option_lower] = create_perturbed_options(
            option, 
            &model::EuropeanOption::implied_volatility
        );
        return calculate_finite_difference_first_order(option_upper, option_lower, h, pricer);
    }

    double theta(const model::EuropeanOption& option, auto&& pricer) {
        auto [h, option_upper, option_lower] = create_perturbed_options(option, &model::EuropeanOption::expiry);
        return -calculate_finite_difference_first_order(option_upper, option_lower, h, pricer);
    }

    double rho(const model::EuropeanOption& option, auto&& pricer) {
        auto [h, option_upper, option_lower] = create_perturbed_options(
            option,
            &model::EuropeanOption::risk_free_rate
        );
        return calculate_finite_difference_first_order(option_upper, option_lower, h, pricer);
    }
}

}
