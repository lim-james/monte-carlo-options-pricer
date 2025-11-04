#ifndef EU_OPTION_OUT_H
#define EU_OPTION_OUT_H

#include "option_type.h"

struct eu_option_out {
    OptionType type;

    double spot;  
    double strike;  
    double expiry;  
    double volatility;  
    double rate;  

    double payoff;

    double delta;
    double gamma;
    double vega;
    double rho;
    double theta;
};

#endif
