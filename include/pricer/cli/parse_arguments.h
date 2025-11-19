#pragma once 

#include "pricer/cli/cli_arguments.h"

#include <optional>

namespace parser::cli {

std::optional<CliArguments> parse_arguments(int argsc, const char* argsv[]);

}
