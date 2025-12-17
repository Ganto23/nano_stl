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
    class UniquePtr {
    public:
        UniquePtr(): _ptr(nullptr) {}
        explicit UniquePtr(T* ptr): _ptr(ptr) {}
        ~UniquePtr(){delete _ptr;}

        UniquePtr(const UniquePtr&) = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        UniquePtr(UniquePtr&& other) noexcept: _ptr(other._ptr) {
            other._ptr = nullptr;
        }
        UniquePtr& operator=(UniquePtr&& other) noexcept {
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


