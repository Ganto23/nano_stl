cmake_minimum_required(VERSION 3.14)
project(NanoSTL)

# --- 1. Global Settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 2. Main Library (nstl) ---
add_library(nstl INTERFACE)
target_include_directories(nstl INTERFACE include)

# --- 3. Unit Testing (GTest) ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(unit_tests tests/test_vector.cpp)
target_link_libraries(unit_tests PRIVATE nstl gtest_main)

# --- 4. Benchmarking (Google Benchmark) ---
FetchContent_Declare(
  googlebenchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(benchmarks benchmarks/bench_vector.cpp)
target_link_libraries(benchmarks PRIVATE nstl benchmark::benchmark)

if(MSVC)
    target_compile_options(benchmarks PRIVATE /O2)
else()
    target_compile_options(benchmarks PRIVATE -O3)
endif()
