#include "io.h"

#include "rapidcsv.h"

std::vector<eu_option> load_options_from_csv(const char* filepath) {
    rapidcsv::Document csv(filepath, rapidcsv::LabelParams(0, -1));

    const size_t no_rows = csv.GetRowCount();
    std::vector<eu_option> options_list{no_rows};

    for (size_t i = 0; i < no_rows; ++i) {
        auto row = csv.GetRow<double>(i);
        
        options_list[i] = eu_option{
            row[OptionCSVColumn::COL_SPOT],
            row[OptionCSVColumn::COL_STRIKE],
            row[OptionCSVColumn::COL_EXPIRY],
            row[OptionCSVColumn::COL_VOLATILITY],
            row[OptionCSVColumn::COL_RATE]
        };
    }

    return options_list;
}
