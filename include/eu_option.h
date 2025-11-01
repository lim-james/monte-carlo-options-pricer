#ifndef EU_OPTION_H
#define EU_OPTION_H

enum class OptionType: char { Call, Put };

struct eu_option {
    OptionType type;
    double spot;  
    double strike;  
    double expiry;  
    double volatility;  
    double rate;  
};

#endif
