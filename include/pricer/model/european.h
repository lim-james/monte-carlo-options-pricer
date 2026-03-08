#pragma once

#include <span>

#include "option_type.h"

namespace pricer::model {

struct EuropeanOption {
    OptionType type;
    double spot;
    double strike;
    double time_to_expiry;
    double implied_volatility;
    double risk_free_rate;
};

using EuropeanOptionsView = std::span<EuropeanOption>;

}
