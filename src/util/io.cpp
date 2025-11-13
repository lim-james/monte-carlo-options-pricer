#include "pricer/util/io.h"

#include <format>
#include <string>

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

[[nodiscard("Loaded options unused")]] 
std::vector<model::EuropeanOption> loadOptionsFromCsv(const char* filepath) {
    rapidcsv::Document csv(filepath, rapidcsv::LabelParams(0, -1));

    const size_t no_rows = csv.GetRowCount();
    std::vector<model::EuropeanOption> options_list{no_rows};

    for (size_t i = 0; i < no_rows; ++i) {
        options_list[i] = model::EuropeanOption{
            static_cast<model::OptionType>(csv.GetCell<int>(OptionCSVColumn::COL_TYPE, i)),
            csv.GetCell<double>(OptionCSVColumn::COL_SPOT, i),
            csv.GetCell<double>(OptionCSVColumn::COL_STRIKE, i),
            csv.GetCell<double>(OptionCSVColumn::COL_EXPIRY, i),
            csv.GetCell<double>(OptionCSVColumn::COL_VOLATILITY, i),
            csv.GetCell<double>(OptionCSVColumn::COL_RATE, i)
        };
    }

    return options_list;
}

void saveOptionsToCsv(
    const char* filepath, 
    const std::vector<model::EuropeanOption>& options,
    const std::vector<double>& payoffs,
    const std::vector<model::OptionGreeks>& greeks
) {
    static const std::vector<std::string> header = {
        "type", 
        "spot", "strike", "expiry", "volatility", "rate", 
        "payoff", 
        "delta", "gamma", "vega", "rho", "theta"
    };

    rapidcsv::Document csv;
    csv.SetRow(-1, header);

    std::string missing_payoff_assertion = std::format(
        "Missing payoffs :: options[{}] != payoffs[{}]",
        options.size(),
        payoffs.size()
    );
    assert(options.size() == payoffs.size() && missing_payoff_assertion.c_str());

    std::string missiing_greek_assertion = std::format(
        "Missing greeks :: options[{}] != greeks[{}]",
        options.size(),
        greeks.size()
    );
    assert(options.size() == greeks.size() && missing_payoff_assertion.c_str());

    for (size_t i = 0; i < options.size(); ++i) {
        const model::EuropeanOption& op = options[i];
        const model::OptionGreeks& g = greeks[i];

        csv.SetCell(OptionCSVColumn::COL_TYPE,       i, static_cast<int>(op.type));
        csv.SetCell(OptionCSVColumn::COL_SPOT,       i, op.spot);
        csv.SetCell(OptionCSVColumn::COL_STRIKE,     i, op.strike);
        csv.SetCell(OptionCSVColumn::COL_EXPIRY,     i, op.expiry);
        csv.SetCell(OptionCSVColumn::COL_VOLATILITY, i, op.volatility);
        csv.SetCell(OptionCSVColumn::COL_RATE,       i, op.rate);

        csv.SetCell(OptionCSVColumn::COL_PAYOFF, i, payoffs[i]);

        csv.SetCell(OptionCSVColumn::COL_DELTA, i, g.delta);
        csv.SetCell(OptionCSVColumn::COL_GAMMA, i, g.gamma);
        csv.SetCell(OptionCSVColumn::COL_VEGA,  i, g.vega);
        csv.SetCell(OptionCSVColumn::COL_THETA, i, g.theta);
        csv.SetCell(OptionCSVColumn::COL_RHO,   i, g.rho);
    }

    csv.Save(filepath);
}

}
}
