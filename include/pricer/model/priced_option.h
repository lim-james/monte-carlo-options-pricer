#pragma once

#include "pricer/model/european.h"
#include "pricer/model/option_greeks.h"

namespace pricer::model {

struct PricedOption {
    double payout;
    OptionGreeks greeks;
    EuropeanOption option;
};

}
