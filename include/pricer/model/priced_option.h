#pragma once

#include "pricer/model/european.h"
#include "pricer/model/option_greeks.h"

namespace pricer::model {

struct PricedOption {
    double payoff;
    OptionGreeks greeks;
    EuropeanOption option;
};

using PricedOptionView = std::span<PricedOption>;

}
