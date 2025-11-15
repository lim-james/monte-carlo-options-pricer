#pragma once

#include "pricer/model/european.h"
#include "pricer/model/option_greeks.h"
#include "pricer/method/greeks/greeks_internal.hpp"

namespace pricer {
namespace method::greeks {
    model::OptionGreeks calculate(const model::EuropeanOption& option, auto&& pricing_fn) {
        double delta = internal::delta(option, pricing_fn);
        double gamma = internal::gamma(option, pricing_fn);
        double vega  = internal::vega (option, pricing_fn);
        double theta = internal::theta(option, pricing_fn);
        double rho   = internal::rho  (option, pricing_fn);
        return model::OptionGreeks{delta, gamma, vega, theta, rho};
    }
}
}
