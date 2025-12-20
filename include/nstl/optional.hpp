#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <utility>
#include <cstring>
#include <type_traits>
#include <algorithm>
#include <compare>

namespace nstl {
    template<typename T>
    class optional {
    public:
        optional(){}
        optional(const optional& other){}
        optional(optional&& other){}

        ~optional(){}

        T& operator=(const optional& other){}
        T& operator=(optional&& other){}

    private:
        T value;
    };
}