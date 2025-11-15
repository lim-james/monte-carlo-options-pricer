#pragma once

#include <vector>
#include <filesystem>

#include "pricer/model/european.h"
#include "pricer/model/option_greeks.h"

namespace pricer {
namespace util {

[[nodiscard("Loaded options unused")]] 
std::vector<model::EuropeanOption> loadOptionsFromCsv(const std::filesystem::path& filepath);

void savePricedOptionsToCsv(
    const std::filesystem::path& filepath, 
    const std::vector<model::EuropeanOption>& options,
    const std::vector<double>& payoffs,
    const std::vector<model::OptionGreeks>& greeks
);

}
}

