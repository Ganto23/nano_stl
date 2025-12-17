#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <utility>
#include <cstring>
#include <type_traits>
#include <algorithm>

namespace nstl {

    template<typename T>
    struct default_delete {
        void operator()(T* ptr) const {delete ptr;}
    };

    template<typename T>
    struct default_delete<T[]> {
        void operator()(T* ptr) const {delete[] ptr;}
    };

    template<typename T, typename Deleter = default_delete<T>>
    class unique_ptr {
    public:
        unique_ptr(): _ptr(nullptr) {}
        explicit unique_ptr(T* ptr): _ptr(ptr) {}
        ~unique_ptr(){if (_ptr) _deleter(_ptr);}

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr(unique_ptr&& other) noexcept: _ptr(other._ptr) {
            other._ptr = nullptr;
        }
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (this != &other){
                if (_ptr) _deleter(_ptr);
                _ptr = other._ptr;
                other._ptr = nullptr;
            }
            return *this;
        }

        T* get() const {return _ptr;}
        T* operator->() const {return _ptr;}
        T& operator*() {return *_ptr;}
        const T& operator*() const {return *_ptr;}

        void reset(T* ptr = nullptr){
            if (_ptr != ptr){
                if (_ptr) _deleter(_ptr);
            }
            _ptr = ptr;
        }
        T* release(){
            T* temp = _ptr;
            _ptr = nullptr;
            return temp;
        }

    private:
        T* _ptr = nullptr;
        [[no_unique_address]] Deleter _deleter;
    };

    template<typename T, typename Deleter>
    class unique_ptr<T[], Deleter> {
    public:
        unique_ptr(): _ptr(nullptr) {}
        explicit unique_ptr(T* ptr): _ptr(ptr) {}
        ~unique_ptr(){if (_ptr) _deleter(_ptr);}

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr(unique_ptr&& other) noexcept: _ptr(other._ptr) {
            other._ptr = nullptr;
        }
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (this != &other){
                if (_ptr) _deleter(_ptr);
                _ptr = other._ptr;
                other._ptr = nullptr;
            }
            return *this;
        }

        T* get() const {return _ptr;}
        T& operator[](size_t idx) {return _ptr[idx];}
        const T& operator[](size_t idx) const {return _ptr[idx];}

        void reset(T* ptr = nullptr){
            if (_ptr != ptr) {
                if (_ptr) _deleter(_ptr);
            }
            _ptr = ptr;
        }
        T* release(){
            T* temp = _ptr;
            _ptr = nullptr;
            return temp;
        }

    private:
        T* _ptr = nullptr;
        [[no_unique_address]] Deleter _deleter;
    };

    template<typename T>
    concept Scalar = !std::is_array_v<T>;

    template<typename T>
    concept UnboundedArray = std::is_array_v<T> && std::extent_v<T> == 0;

    template<typename T>
    concept BoundedArray = std::is_array_v<T> && std::extent_v<T> != 0;

    template<typename T, typename... Args>
    requires Scalar<T>
    unique_ptr<T> make_unique(Args&&... args){
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    };

    template<typename T>
    requires UnboundedArray<T>
    unique_ptr<T> make_unique(size_t size){
        using ElemType = std::remove_extent_t<T>;
        return unique_ptr<T>(new ElemType[size]());
    };
}


