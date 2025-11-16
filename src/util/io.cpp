#include "pricer/util/io.h"

#include <format>
#include <string>
#include <ranges>
#include <functional>

#include "pricer/model/european.h"
#include "pricer/model/option_greeks.h"
#include "pricer/util/rapidcsv.h"
#include "pricer/model/option_type.h"

namespace pricer {
namespace util {

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

model::EuropeanOption parseOptionFromRow(const rapidcsv::Document& csv, size_t row_index) {
    return model::EuropeanOption{
        static_cast<model::OptionType>(
            csv.GetCell<int>(OptionCSVColumn::COL_TYPE, row_index)
        ),
        csv.GetCell<double>(OptionCSVColumn::COL_SPOT, row_index),
        csv.GetCell<double>(OptionCSVColumn::COL_STRIKE, row_index),
        csv.GetCell<double>(OptionCSVColumn::COL_EXPIRY, row_index),
        csv.GetCell<double>(OptionCSVColumn::COL_VOLATILITY, row_index),
        csv.GetCell<double>(OptionCSVColumn::COL_RATE, row_index)
    };
}

[[nodiscard("Loaded options unused")]] 
std::vector<model::EuropeanOption> loadOptionsFromCsv(const std::filesystem::path& filepath) {
    rapidcsv::Document csv(filepath, rapidcsv::LabelParams(0, -1));

    return std::views::iota(0U, csv.GetRowCount()) 
        | std::views::transform(std::bind_front(parseOptionFromRow, std::ref(csv))) 
        | std::ranges::to<std::vector>();
}

void writeHeaders(rapidcsv::Document& csv) {
    static const std::vector<std::string> header = {
        "type", 
        "spot", "strike", "expiry", "volatility", "rate", 
        "payoff", 
        "delta", "gamma", "vega", "rho", "theta"
    };

    csv.SetRow(-1, header);
}

void writeOption(
    rapidcsv::Document& csv, 
    size_t row_index, 
    const model::EuropeanOption& option
) {
    csv.SetCell(OptionCSVColumn::COL_TYPE,       row_index, static_cast<int>(option.type));
    csv.SetCell(OptionCSVColumn::COL_SPOT,       row_index, option.spot);
    csv.SetCell(OptionCSVColumn::COL_STRIKE,     row_index, option.strike);
    csv.SetCell(OptionCSVColumn::COL_EXPIRY,     row_index, option.expiry);
    csv.SetCell(OptionCSVColumn::COL_VOLATILITY, row_index, option.volatility);
    csv.SetCell(OptionCSVColumn::COL_RATE,       row_index, option.rate);
}

void writeOptionPayoff(
    rapidcsv::Document& csv, 
    size_t row_index, 
    double payoff
) {
}

void writeOptionGreeks(
    rapidcsv::Document& csv, 
    size_t row_index,
    const model::OptionGreeks& greeks
) {
    csv.SetCell(OptionCSVColumn::COL_DELTA, row_index, greeks.delta);
    csv.SetCell(OptionCSVColumn::COL_GAMMA, row_index, greeks.gamma);
    csv.SetCell(OptionCSVColumn::COL_VEGA,  row_index, greeks.vega);
    csv.SetCell(OptionCSVColumn::COL_THETA, row_index, greeks.theta);
    csv.SetCell(OptionCSVColumn::COL_RHO,   row_index, greeks.rho);
}

//void writeResult(
//    rapidcsv::Document& csv,
//    size_t row_index,
//)

void savePricedOptionsToCsv(
    const std::filesystem::path& filepath, 
    const std::vector<model::PricedOption>& options
) {
    rapidcsv::Document csv;    
    writeHeaders(csv);

    for (size_t i = 0; i < options.size(); ++i) {
        writeOption(csv, i, options[i].option);
        writeOptionGreeks(csv, i, options[i].greeks);
        csv.SetCell(OptionCSVColumn::COL_PAYOFF, i, options[i].payoff);
    }

    csv.Save(filepath);
}

}
}
