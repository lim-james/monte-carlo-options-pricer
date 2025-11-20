#include "pricer/cli/parse_arguments.h"

#include <thread>

namespace pricer::cli {

std::optional<CliArguments> parse_arguments(int argsc, const char* argsv[]) {
    if (argsc <= 1) {
        return std::nullopt;
    }

    constexpr std::size_t DEFAULT_SAMPLE_COUNT = 1'000'000;
    const std::size_t MAX_THREADS  = std::thread::hardware_concurrency();

    std::size_t sample_count = argsc > 2 ? std::stol(argsv[2]) : DEFAULT_SAMPLE_COUNT;
    std::size_t thread_count = argsc > 3 ? std::stoi(argsv[3]) : MAX_THREADS;

    return CliArguments{argsv[1], sample_count, thread_count}; 
}

}
