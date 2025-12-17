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
    class unique_ptr {
    public:
        unique_ptr(): _ptr(nullptr) {}
        explicit unique_ptr(T* ptr): _ptr(ptr) {}
        ~unique_ptr(){delete _ptr;}

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr(unique_ptr&& other) noexcept: _ptr(other._ptr) {
            other._ptr = nullptr;
        }
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (this != &other){
                delete _ptr;
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
            if (_ptr != ptr) delete _ptr;
            _ptr = ptr;
        }
        T* release(){
            T* temp = _ptr;
            _ptr = nullptr;
            return temp;
        }

    private:
        T* _ptr = nullptr;
    };
}


