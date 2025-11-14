#pragma once

namespace pricer::model {

struct PerfStats {
    double total_ms        = 0.0;
    double mean_ms         = 0.0;
    double std_ms          = 0.0;
    double options_per_min = 0.0;
};

}
