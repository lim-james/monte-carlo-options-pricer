#pragma once

#include "pricer/model/european.h"
#include <concepts>

namespace pricer {
namespace method {

template<typename P>
concept OptionPricer = requires(const P& p, const model::EuropeanOption& option) {
    { p.price(option) } -> std::convertible_to<double>;
};

}
}
