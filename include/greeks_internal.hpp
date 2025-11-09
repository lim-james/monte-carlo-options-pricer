#ifndef GREEKS_INTERNAL_H
#define GREEKS_INTERNAL_H

#include "eu_option.h"

#include <vector>
#include <cmath>

namespace greeks::internal {
    inline double eval_finite_step(double value) {
        return 0.01 * value;
    }

    inline double eval_finite_difference_first(
        const eu_option& up, 
        const eu_option& dn,
        double h,
        auto&& pricing_fn
    ) {
        const double up_payoff = pricing_fn(up);   
        const double dn_payoff = pricing_fn(dn);   
        return (up_payoff - dn_payoff) / (2 * h);
    }

    double delta(const eu_option& option, auto&& pricing_fn) {
        const double h = eval_finite_step(option.spot);
        eu_option option_up = option;
        option_up.spot += h;

        eu_option option_dn = option;
        option_dn.spot -= h;

        return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }

    double gamma(const eu_option& option, auto&& pricing_fn) {
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

    double vega(const eu_option& option, auto&& pricing_fn) {
        const double h = eval_finite_step(option.volatility);
        eu_option option_up = option;
        option_up.volatility += h;

        eu_option option_dn = option;
        option_dn.volatility -= h;

        return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }

    double theta(const eu_option& option, auto&& pricing_fn) {
        const double h = eval_finite_step(option.expiry);
        eu_option option_up = option;
        option_up.expiry += h;

        eu_option option_dn = option;
        option_dn.expiry -= h;

        return -eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }

    double rho(const eu_option& option, auto&& pricing_fn) {
        const double h = eval_finite_step(option.rate);
        eu_option option_up = option;
        option_up.rate += h;

        eu_option option_dn = option;
        option_dn.rate -= h;

        return eval_finite_difference_first(option_up, option_dn, h, pricing_fn);
    }
}

#endif
