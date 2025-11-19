#pragma once

#include <ranges>
#include <cmath>
#include <algorithm>

namespace pricer::util {

template<std::ranges::input_range FirstRange, std::ranges::input_range SecondRange>
double mean_absolute_difference(FirstRange&& first_range, SecondRange&& second_range) {
    assert(std::ranges::distance(first_range) == std::ranges::distance(second_range) 
           && "Mismatching sizes when computing mean absolute difference");

    auto pairwise_differences = std::views::zip(first_range, second_range) 
        | std::views::transform([](const auto& pair) {
            const auto& [a, b] = pair; 
            return std::fabs(a - b);
        });

    double accumulated_diffs = std::ranges::fold_left(pairwise_differences, 0.0, std::plus{});
    std::size_t number_of_elements = std::ranges::distance(pairwise_differences);

    return accumulated_diffs / number_of_elements;
}

}

