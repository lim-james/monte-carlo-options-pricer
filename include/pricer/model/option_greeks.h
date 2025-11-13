#ifndef OPTION_GREEKS_H
#define OPTION_GREEKS_H

namespace pricer::model {

struct OptionGreeks {
    double delta;
    double gamma;
    double vega;
    double rho;
    double theta;
};

}

#endif
