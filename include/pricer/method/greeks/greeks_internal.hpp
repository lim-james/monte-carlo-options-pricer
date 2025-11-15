#pragma once

#include "pricer/model/european.h"

namespace pricer {
namespace method::greeks::internal {
    inline double eval_finite_step(double value) {
        return 0.01 * value;
    }

    inline double eval_finite_difference_first(
        const model::EuropeanOption& up, 
        const model::EuropeanOption& dn,
        double h,
        auto&& pricing_fn
    ) {
        double up_payoff = pricing_fn(up);   
        double dn_payoff = pricing_fn(dn);   
        return (up_payoff - dn_payoff) / (2 * h);
    }

    double delta(const model::EuropeanOption& option, auto&& pricing_fn) {
        double h = eval_finite_step(option.spot);
        model::EuropeanOption option_up = option;
        option_up.spot += h;

        model::EuropeanOption option_dn = option;
        option_dn.spot -= h;

        return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }

    double gamma(const model::EuropeanOption& option, auto&& pricing_fn) {
        double h = eval_finite_step(option.spot);
        model::EuropeanOption option_up = option;
        option_up.spot += h;

        model::EuropeanOption option_dn = option;
        option_dn.spot -= h;

        double payoff = pricing_fn(option);
        double up_payoff = pricing_fn(option_up);   
        double dn_payoff = pricing_fn(option_dn);   
        return (up_payoff - 2 * payoff + dn_payoff) / (h * h);
    }

    double vega(const model::EuropeanOption& option, auto&& pricing_fn) {
        double h = eval_finite_step(option.volatility);
        model::EuropeanOption option_up = option;
        option_up.volatility += h;

        model::EuropeanOption option_dn = option;
        option_dn.volatility -= h;

        return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }

    double theta(const model::EuropeanOption& option, auto&& pricing_fn) {
        const double h = eval_finite_step(option.expiry);
        model::EuropeanOption option_up = option;
        option_up.expiry += h;

        model::EuropeanOption option_dn = option;
        option_dn.expiry -= h;

        return -eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }

    double rho(const model::EuropeanOption& option, auto&& pricing_fn) {
        const double h = eval_finite_step(option.rate);
        model::EuropeanOption option_up = option;
        option_up.rate += h;

        model::EuropeanOption option_dn = option;
        option_dn.rate -= h;

        return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }
}

}
