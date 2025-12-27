#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <utility>
#include <cstring>
#include <type_traits>
#include <algorithm>
#include <compare>
#include <limits>

namespace nstl {
    inline constexpr size_t dynamic_extent = std::numeric_limits<size_t>::max();

    template <typename T, size_t Extent = dynamic_extent>
    class span {
    public:
        span(){}
        span(T* ptr, size_t count){}
        span(const span& other){}
        span(span&& other){}

        ~span(){}

        span& operator=(const span& other){}
        span& operator=(span&& other){}

        size_t size() {}
        T* data() {}
        T& operator[]() {} 

    private:
        T* _ptr;
        size_t _size;
    };

    template <typename T, size_t Extent>
    requires (Extent != dynamic_extent)
    class span {
    
    };
}