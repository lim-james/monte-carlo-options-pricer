#pragma once

namespace pricer::model {

struct OptionGreeks {
    double delta;
    double gamma;
    double vega;
    double rho;
    double theta;
};

}
