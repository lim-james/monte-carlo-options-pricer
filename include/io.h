#ifndef IO_H
#define IO_H

#include <vector>

#include "eu_option.h"
#include "eu_option_out.h"

[[nodiscard("Loaded options unused")]] 
std::vector<eu_option> load_options_from_csv(const char* filepath);

void save_options_to_csv(
    const char* filepath, 
    const std::vector<eu_option_out>& options
);

#endif
