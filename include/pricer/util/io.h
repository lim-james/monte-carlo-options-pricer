#pragma once

#include <vector>
#include <filesystem>

#include "pricer/model/european.h"
#include "pricer/model/priced_option.h"

namespace pricer {
namespace util {

[[nodiscard("Loaded options unused")]] 
std::vector<model::EuropeanOption> load_options_from_csv(const std::filesystem::path& filepath);

void save_priced_options_to_csv(
    const std::filesystem::path& filepath, 
    model::PricedOptionView options
);

}
}

