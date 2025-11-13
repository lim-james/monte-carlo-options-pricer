#pragma once

#include "option_type.h"

namespace pricer::model {

struct EuropeanOption {
    OptionType type;
    double spot;  
    double strike;  
    double expiry;  
    double volatility;  
    double rate;  
};

}
