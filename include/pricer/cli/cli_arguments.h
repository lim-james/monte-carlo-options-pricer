#pragma once 

#include <filesystem>

namespace parser::cli {

struct CliArguments {
    std::filesystem::path filepath;
    std::size_t sample_count;
    std::size_t thread_count;
};

}
