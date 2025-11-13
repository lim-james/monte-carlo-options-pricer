#pragma once

#include "pricer/model/european.h"

namespace pricer {
namespace method {

class OptionPricer {
public:
    virtual ~OptionPricer() = default;
    virtual double price(const model::EuropeanOption& option) const = 0;
};

}
}
