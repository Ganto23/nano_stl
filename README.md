# NSTL - High-Performance STL Implementation

NSTL delivers standards-compliant C++ containers optimized for low-latency HFT workloads with 6.8x faster small vector operations.  

## 📋 Table of Contents
- [Vector](#vector)
- [Optional](#optional)
- [UniquePtr](#uniqueptr)
- [Span](#span)
- [Building & Testing](#building)

## 🧮 Vector

### Overview
High-performance dynamic array with geometric capacity growth (capacity × 2 or max(8, capacity × 2)). Matches std::vector semantics with superior small-size performance.  

### Implementation Details
- Trivially copyable fast-paths using memcpy
- std::allocator<T> with [[no_unique_address]]
- In-place emplace_back() via std::construct_at
- Full random-access iterators (begin/end/cbegin/cend)  

### Core Operations
- push_back(value) → O(1) amortized
- emplace_back(args...) → O(1) amortized, returns T&
- operator → O(1)
- reserve(n), shrink_to_fit()


### Testing Coverage
- 7 test cases: CRUD operations, bounds checking, capacity growth verification, deep copy, emplace-back with self-referential edge cases  

### Benchmarks
| Size | std::vector | nstl::vector | Speedup |
|------|-------------|--------------|---------|
| 8    | 154 ns      | **22.6 ns**  | **6.8x** |
| 8192 | 3838 ns     | 3965 ns      | ~1x     |  

## ✅ Optional

### Overview
Type-safe optional<T> with sizeof(optional<T>) == sizeof(T) for trivial types using union-based storage.  

### Implementation Details
union {
char dummy[sizeof(T)];
T value;
};
bool engaged = false;

Perfect forwarding constructors, strong exception safety, swap with noexcept guarantees.  

### Core Operations
value_or(default), emplace(args...), reset(), swap()
Throws std::bad_optional_access when empty


### Testing Coverage
- 12 test cases: type traits matching std::optional, move/copy semantics, lifetime tracking, swap behaviors, exception safety  

## 🔒 UniquePtr

### Overview
Move-only ownership semantics with customizable deleters and full array support (T[] and T[N]).  

### Implementation Details
template<typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
T* ptr = nullptr;
[[no_unique_address]] Deleter deleter;
};

Supports make_unique<T>(args...), operator[](size_t) for arrays.  

### Core Operations
- get(), release(), reset(), operator*()->, custom deleters

### Testing Coverage
- 15 test cases: move-only semantics, custom deleters, array indexing, RAII lifetime management, nullptr behavior  

## 📐 Span

### Overview
Non-owning view over contiguous sequences with zero-overhead design principles matching other NSTL components.  

### Implementation Details
Follows C++20 std::span semantics with identical performance characteristics to reference implementation.  

## 🔨 Building & Testing

- mkdir build && cd build
- cmake .. -DCMAKE_CXX_STANDARD=23
- make -j$(nproc)
- ctest -V # Run all tests
- ./benchmarks/bench_vector


**Requirements:** C++23, GCC 14+, Clang 18+, MSVC 2022+, GoogleTest, Google Benchmark  
