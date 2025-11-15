cmake_minimum_required(VERSION 3.23)
project(MonteCarloOptionsPricer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(MonteCarloOptionsPricer)
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
target_sources(MonteCarloOptionsPricer PRIVATE ${CPP_SOURCES})
