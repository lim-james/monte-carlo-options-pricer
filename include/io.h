#ifndef IO_H
#define IO_H

#include <vector>

#include "eu_option.h"

std::vector<eu_option> load_options_from_csv(const char* filepath);
void save_payoffs_to_csv(const char* filepath, const std::vector<double>& payoffs);

#endif
