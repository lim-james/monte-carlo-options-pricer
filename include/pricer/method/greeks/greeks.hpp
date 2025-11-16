#pragma once

#include "pricer/model/european.h"
#include "pricer/model/option_greeks.h"
#include "pricer/method/greeks/greeks_internal.hpp"

namespace pricer {
namespace method::greeks {
    model::OptionGreeks calculate(const model::EuropeanOption& option, auto&& pricer) {
        double delta = internal::delta(option, pricer);
        double gamma = internal::gamma(option, pricer);
        double vega  = internal::vega (option, pricer);
        double theta = internal::theta(option, pricer);
        double rho   = internal::rho  (option, pricer);
        return model::OptionGreeks{delta, gamma, vega, theta, rho};
    }
}
}
