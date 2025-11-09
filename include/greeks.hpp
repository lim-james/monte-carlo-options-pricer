#ifndef GREEKS_H
#define GREEKS_H

#include "option_greeks.h"
#include "greeks_internal.hpp"

namespace greeks {
    option_greeks calculate(const eu_option& option, auto&& pricing_fn) {
        double delta = internal::delta(option, pricing_fn);
        double gamma = internal::gamma(option, pricing_fn);
        double vega  = internal::vega (option, pricing_fn);
        double theta = internal::theta(option, pricing_fn);
        double rho   = internal::rho  (option, pricing_fn);
        return option_greeks{delta, gamma, vega, theta, rho};
    }
}

#endif
