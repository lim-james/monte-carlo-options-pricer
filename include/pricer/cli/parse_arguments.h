#pragma once 

#include "pricer/cli/cli_arguments.h"

#include <optional>

namespace pricer::cli {

std::optional<CliArguments> parse_arguments(int argsc, const char* argsv[]);

}
