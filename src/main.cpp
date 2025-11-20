#include <print>
#include <cassert>

#include "pricer/cli/parse_arguments.h"
#include "pricer/app.h"


int main(int argsc, const char* argsv[]) {
    if (auto arguments = pricer::cli::parse_arguments(argsc, argsv)) {
        return pricer::run(*arguments);
    }

    std::println("Please provide a .csv filepath");
    return 0;
}
