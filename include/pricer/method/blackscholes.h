#pragma once

#include "pricer/model/european.h"
#include "pricer/method/option_pricer.h"

namespace pricer {
namespace method::blackscholes {

class BlackScholesPricer {
public:
    double price(const model::EuropeanOption& option) const;
};

}
}

