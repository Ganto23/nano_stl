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
    struct nullopt_t {
        explicit constexpr nullopt_t(int) {}
    };
    inline constexpr nullopt_t nullopt{0};

    template<typename T>
    class optional {
    public:
        // CONSTRUCTORS
        constexpr optional() noexcept;
        constexpr optional(nullopt_t) noexcept;
        constexpr optional(const T& value);
        constexpr optional(T&& value);
        template<class U> constexpr optional(const optional<U>&);
        template<class U> constexpr optional(optional<U>&&);
        constexpr optional(const optional& other){}
        constexpr optional(optional&& other){}

        ~optional(){}

        //Assignment and modifiers
        constexpr optional& operator=(nullopt_t) noexcept;
        constexpr optional& operator=(const optional&);
        constexpr optional& operator=(optional&&);
        template<class U = T> 
        constexpr optional& operator=(U&& value);


        template<class... Args> 
        T& emplace(Args&&... args);

        constexpr void reset() noexcept;
        constexpr void swap(optional& other) noexcept(...);

        //Observers
        constexpr bool has_value() const noexcept;
        constexpr explicit operator bool() const noexcept;

        constexpr T& value();
        constexpr const T& value() const;
        constexpr T&& value() const noexcept;

        constexpr T& operator*() &;
        constexpr const T& operator*() const &;
        constexpr T&& operator*() const noexcept &;

        constexpr T* operator->() noexcept;
        constexpr const T* operator->() const noexcept;

        template<class U> 
        constexpr T value_or(U&& default_value) const &;
        template<class U> 
        constexpr T value_or(U&& default_value) &&;        

    private:
        union {
            char dummy;
            T   value;
        };
        bool engaged_ = false;
    };
}