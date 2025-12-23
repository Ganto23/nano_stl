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
        explicit constexpr nullopt_t() = default;
    };
    inline constexpr nullopt_t nullopt{};

    template<typename T>
    class optional {
    public:
        // CONSTRUCTORS
        constexpr optional() noexcept : dummy_{}, engaged_(false){}
        constexpr optional(nullopt_t) noexcept : dummy_{}, engaged_(false){}
        constexpr optional(const T& value) : value_(value), engaged_(true){}
        constexpr optional(T&& value) : value_(std::move(value)), engaged_(true){}
        template<class U> 
        constexpr optional(const optional<U>& other) : dummy_{}, engaged_(false) {
            if (other.has_value()){
                new (&value_) T(other.value());
                engaged_ = true;
            }
        }
        template<class U> 
        constexpr optional(optional<U>&& other) : dummy_{}, engaged_(false) {
            if (other.has_value()){
                new (&value_) T(std::move(other.value()));
                engaged_ = true;
            }
        }
        constexpr optional(const optional& other) : dummy_{}, engaged_(false) {
            if (other.has_value()){
                new (&value_) T(other.value());
                engaged_ = true;
            }
        }
        constexpr optional(optional&& other) : dummy_{}, engaged_(false) {
            if (other.has_value()){
                new (&value_) T(std::move(other.value()));
                engaged_ = true;
            }
        }

        ~optional(){
            if (engaged_) {
                value_.~T();
            }
        }

        //Assignment and modifiers
        constexpr optional& operator=(nullopt_t) noexcept {
            reset();
            return *this;
        }
        constexpr optional& operator=(const optional& other) {
            reset();
            if (other.has_value()){
                new (&value_) T(other.value());
                engaged_ = true;
            }
            return *this;
        }
        constexpr optional& operator=(optional&& other) {
            reset();
            if (other.has_value()){
                new (&value_) T(std::move(other.value()));
                engaged_ = true;
            }
            return *this;
        }
        template<class U = T, std::enable_if_t<!std::is_same_v<std::remove_cv_t<std::remove_reference_t<U>>, optional>, int> = 0> 
        constexpr optional& operator=(U&& value) {
            if (engaged_){
                value_ = std::forward<U>(value);
            } else {
                new (&value_) T(std::forward<U>(value));
                engaged_ = true;
            }
            return *this;
        }


        template<class... Args> 
        T& emplace(Args&&... args) {
            reset();
            new (&value_) T(std::forward<Args>(args)...);
            engaged_ = true;
            return value_;
        }

        constexpr void reset() noexcept {
            if (engaged_){
                value_.~T();
                engaged_ = false;
            }
        }
        constexpr void swap(optional& other) noexcept(
            std::is_nothrow_move_constructible_v<T> &&
            std::is_nothrow_swappable_v<T>
        ) {
            using std::swap;

            if (engaged_ && other.engaged_) {
                swap(value_, other.value_);
            } else if (engaged_ && !other.engaged_) {
                new (&other.value_) T(std::move(value_));
                other.engaged_ = true;
                reset();
            } else if (!engaged_ && other.engaged_) {
                new (&value_) T(std::move(other.value_));
                engaged_ = true;
                other.reset();
            }
        }

        //Observers
        constexpr bool has_value() const noexcept {return engaged_;}
        constexpr explicit operator bool() const noexcept {return engaged_;}

        constexpr T& value() {
            if (engaged_) return value_;
            throw std::bad_optional_access{};
        }
        constexpr const T& value() const {
            if (engaged_) return value_;
            throw std::bad_optional_access{};
        }

        constexpr T& operator*() & {
            if (!engaged_) throw std::bad_optional_access{};
            return value_;
        }
        constexpr const T& operator*() const & {
            if (!engaged_) throw std::bad_optional_access{};
            return value_;
        }
        constexpr T&& operator*() && noexcept {return std::move(value_);}

        constexpr T* operator->() noexcept {return &value_;}
        constexpr const T* operator->() const noexcept {return &value_;}

        template<class U> 
        constexpr T value_or(U&& default_value) const & {
            if (engaged_){
                return value_;
            } else {
                return static_cast<T>(std::forward<U>(default_value)); 
            }
        }
        template<class U> 
        constexpr T value_or(U&& default_value) && {
            if (engaged_) {
                return std::move(value_);
            } else {
                return static_cast<T>(std::forward<U>(default_value));
            }
        }    

    private:
        union {
            char dummy_;
            T   value_;
        };
        bool engaged_ = false;
    };
}