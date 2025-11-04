#include "io.h"

#include "rapidcsv.h"

enum OptionCSVColumn {
    COL_TYPE,
    COL_SPOT,
    COL_STRIKE,
    COL_EXPIRY,
    COL_VOLATILITY,
    COL_RATE,
    COL_PAYOFF,
    COL_DELTA,
    COL_GAMMA,
    COL_VEGA,
    COL_RHO,
    COL_THETA
};

[[nodiscard("Loaded options unused")]] 
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

void save_options_to_csv(
    const char* filepath, 
    const std::vector<eu_option_out>& options
) {
    static const std::vector<std::string> header = {
        "type", 
        "spot", "strike", "expiry", "volatility", "rate", 
        "payoff", 
        "delta", "gamma", "vega", "rho", "theta"
    };

    rapidcsv::Document csv;
    csv.SetRow(-1, header);

    for (size_t i = 0; i < options.size(); ++i) {
        const eu_option_out& op = options[i];
        csv.SetCell(OptionCSVColumn::COL_TYPE, i, static_cast<int>(op.type));
        csv.SetCell(OptionCSVColumn::COL_SPOT, i, op.spot);
        csv.SetCell(OptionCSVColumn::COL_STRIKE, i, op.strike);
        csv.SetCell(OptionCSVColumn::COL_EXPIRY, i, op.expiry);
        csv.SetCell(OptionCSVColumn::COL_VOLATILITY, i, op.volatility);
        csv.SetCell(OptionCSVColumn::COL_RATE, i, op.rate);
        csv.SetCell(OptionCSVColumn::COL_PAYOFF, i, op.payoff);
        csv.SetCell(OptionCSVColumn::COL_DELTA, i, op.delta);
        csv.SetCell(OptionCSVColumn::COL_GAMMA, i, op.gamma);
        csv.SetCell(OptionCSVColumn::COL_VEGA, i, op.vega);
        csv.SetCell(OptionCSVColumn::COL_THETA, i, op.theta);
        csv.SetCell(OptionCSVColumn::COL_RHO, i, op.rho);
    }

    csv.Save(filepath);
}
