#ifndef EUROPEAN_OPTION_H
#define EUROPEAN_OPTION_H

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

#endif
