#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>

namespace nstl {
    template<typename T>
    class vector{
    public:
        vector(size_t initial_capacity = 1){
            
        }
        vector(const vector&) = delete;
        vector& operator=(const vector&) = delete;
        vector(vector&&) = default;
        vector& operator=(vector&&) = default;

        void push_back(T value){
            
        }
        T pop_back(){
            
        }
        void insert(T value, size_t idx){
            
        }
        T operator[](size_t idx){
            
        }

    private:
        T* vector;
        size_t capacity;
        size_t length;
        std::allocator<T> allocator;

        void resize(){
            
        }
        void clear(){
            
        }
    };
}