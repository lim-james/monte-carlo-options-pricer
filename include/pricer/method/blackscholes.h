#ifndef BLACKSCHOLES_METHOD 
#define BLACKSCHOLES_METHOD 

#include "pricer/model/european.h"

namespace pricer {
namespace method::blackscholes {

double priceOption(const model::EuropeanOption& option);

}
}

#endif
