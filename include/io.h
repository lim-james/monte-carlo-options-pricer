#ifndef IO_H
#define IO_H

#include <vector>

#include "eu_option.h"

enum OptionCSVColumn {
    COL_SPOT,
    COL_STRIKE,
    COL_EXPIRY,
    COL_VOLATILITY,
    COL_RATE,
};

std::vector<eu_option> load_options_from_csv(const char* filepath);

#endif
