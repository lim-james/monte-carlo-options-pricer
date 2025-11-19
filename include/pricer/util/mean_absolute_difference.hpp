#pragma once

#include <ranges>
#include <cmath>
#include <algorithm>

namespace pricer::util {

template<std::ranges::input_range FirstRange, std::ranges::input_range SecondRange>
double mean_absolute_difference(FirstRange&& first_range, SecondRange&& second_range) {
    assert(first_range.size() == second_range.size() 
           && "Mismatching sizes when computing mean absolute difference");

    auto diffs = std::views::zip(first_range, second_range) 
        | std::views::transform([](const auto& zip) {
            const auto& [a, b] = zip; 
            return a - b;
        });

    double accumulated_diffs = std::ranges::fold_left(diffs, 0.0, std::plus{});
    std::size_t number_of_elements = std::ranges::distance(diffs);

    return std::fabs(accumulated_diffs / number_of_elements);
}

}

