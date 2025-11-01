#include "io.h"

#include "rapidcsv.h"

enum OptionCSVColumn {
    COL_TYPE,
    COL_SPOT,
    COL_STRIKE,
    COL_EXPIRY,
    COL_VOLATILITY,
    COL_RATE,
};

std::vector<eu_option> load_options_from_csv(const char* filepath) {
    rapidcsv::Document csv(filepath, rapidcsv::LabelParams(0, -1));

    const size_t no_rows = csv.GetRowCount();
    std::vector<eu_option> options_list{no_rows};

    for (size_t i = 0; i < no_rows; ++i) {
        options_list[i] = eu_option{
            static_cast<OptionType>(csv.GetCell<int>(OptionCSVColumn::COL_TYPE, i)),
            csv.GetCell<double>(OptionCSVColumn::COL_SPOT, i),
            csv.GetCell<double>(OptionCSVColumn::COL_STRIKE, i),
            csv.GetCell<double>(OptionCSVColumn::COL_EXPIRY, i),
            csv.GetCell<double>(OptionCSVColumn::COL_VOLATILITY, i),
            csv.GetCell<double>(OptionCSVColumn::COL_RATE, i)
        };
    }

    return options_list;
}
