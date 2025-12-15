#include <benchmark/benchmark.h>
#include <vector>
#include <nstl/vector.hpp>
#include <random>

// ---------------------------------------------------
// Benchmark 1: STL push_back
// ---------------------------------------------------
static void BM_StdVector_PushBack(benchmark::State& state) {
    // state is an iterator. It runs the loop repeatedly until
    // it gets a statistically significant measurement.
    for (auto _ : state) {
        
        // Setup: Create a fresh vector every iteration
        std::vector<int> v;
        
        // The Work: Push 'N' elements
        // state.range(0) comes from the ->Range() call below
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
        
        // Teardown: v is destroyed here. 
        // Destruction time IS included in the measurement, which is fair
        // because we want to measure the full lifecycle cost.
    }
}

// Register the function as a benchmark
// ->Range(8, 8<<10) means: Run with N = 8, 64, 512, 4096, 8192
BENCHMARK(BM_StdVector_PushBack)->Range(8, 8<<10);


// ---------------------------------------------------
// Benchmark 2: NSTL push_back
// ---------------------------------------------------
static void BM_NstlVector_PushBack(benchmark::State& state) {
    for (auto _ : state) {
        nstl::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
    }
}
BENCHMARK(BM_NstlVector_PushBack)->Range(8, 8<<10);

// ---------------------------------------------------
// Benchmark 3: STL random access
// ---------------------------------------------------
static void BM_StdVector_RandomAccess(benchmark::State& state){
    size_t N = state.range(0);
    std::vector<int> v;
    v.reserve(N);
    for (int i = 0; i < N; ++i) v.push_back(i);

    std::vector<int> random_indices;
    random_indices.reserve(100000); // Create a batch of lookups
    
    std::mt19937 rng(12345); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> dist(0, N - 1);
    
    for(int i=0; i<100000; ++i) {
        random_indices.push_back(dist(rng));
    }

    long long sum = 0;
    size_t idx = 0;

    for (auto _ : state){
        int rand_pos = random_indices[idx % 100000];
        sum += v[rand_pos];
        idx++;
    }
    benchmark::DoNotOptimize(sum);
}
BENCHMARK(BM_StdVector_RandomAccess)->Range(8, 8<<10);

// ---------------------------------------------------
// Benchmark 4: NSTL random access
// ---------------------------------------------------
static void BM_NstlVector_RandomAccess(benchmark::State& state){
    size_t N = state.range(0);
    nstl::vector<int> v;
    v.reserve(N);
    for (int i = 0; i < N; ++i) v.push_back(i);

    std::vector<int> random_indices;
    random_indices.reserve(100000); // Create a batch of lookups
    
    std::mt19937 rng(12345); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> dist(0, N - 1);
    
    for(int i=0; i<100000; ++i) {
        random_indices.push_back(dist(rng));
    }

    long long sum = 0;
    size_t idx = 0;

    for (auto _ : state){
        int rand_pos = random_indices[idx % 100000];
        sum += v[rand_pos];
        idx++;
    }
    benchmark::DoNotOptimize(sum);
}
BENCHMARK(BM_NstlVector_RandomAccess)->Range(8, 8<<10);




struct Heavy {
    std::string s;    
    Heavy() : s("Initial string to prevent SSO optimizations") {}
    Heavy(const char* c) : s(c) {}
};


// ---------------------------------------------------
// Benchmark 5: STL Heavy Push_back
// ---------------------------------------------------
static void BM_StdVector_PushBack_Heavy(benchmark::State& state){
    Heavy proto("Long string to defeat Small String Optimization");
    for (auto _ : state) {
        std::vector<Heavy> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(proto);
        }
    }
}
BENCHMARK(BM_StdVector_PushBack_Heavy)->Range(8, 1<<10);


// ---------------------------------------------------
// Benchmark 6: NSTL Heavy Push_back
// ---------------------------------------------------
static void BM_NstlVector_PushBack_Heavy(benchmark::State& state){
    Heavy proto("Long string to defeat Small String Optimization");
    for (auto _ : state) {
        nstl::vector<Heavy> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(proto);
        }
    }
}
BENCHMARK(BM_NstlVector_PushBack_Heavy)->Range(8, 1<<10);

BENCHMARK_MAIN();

