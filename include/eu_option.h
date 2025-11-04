#ifndef EU_OPTION_H
#define EU_OPTION_H

#include "option_type.h"

struct eu_option {
    OptionType type;
    double spot;  
    double strike;  
    double expiry;  
    double volatility;  
    double rate;  
};

#endif
