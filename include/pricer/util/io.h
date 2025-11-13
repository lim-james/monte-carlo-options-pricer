#pragma once

#include <vector>

#include "pricer/model/european.h"
#include "pricer/model/option_greeks.h"

namespace pricer {
namespace util {

[[nodiscard("Loaded options unused")]] 
std::vector<model::EuropeanOption> loadOptionsFromCsv(const char* filepath);

void saveOptionsToCsv(
    const char* filepath, 
    const std::vector<model::EuropeanOption>& options,
    const std::vector<double>& payoffs,
    const std::vector<model::OptionGreeks>& greeks
);

}
}

