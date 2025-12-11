#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>

namespace nstl {
    template<typename T>
    class vector{
    public:
        vector(size_t initial_capacity = 1){
            capacity = initial_capacity;
            length = 0;
            data = allocator.allocate(capacity);
        }
        ~vector(){
            clear();
            if (data){
                allocator.deallocate(data, capacity);
            }
        }
        vector(const vector& other): capacity(other.capacity), length(other.length) {
            data = allocator.allocate(capacity);
            std::uninitialized_copy(other.data, other.data + other.length, data);
        }
        vector& operator=(vector other) {
            std::swap(data, other.data);
            std::swap(capacity, other.capacity);
            std::swap(length, other.length);
            return *this;
        }

        vector(vector&& other) noexcept : capacity(other.capacity), length(other.length), data(other.data) {
            other.capacity = 0;
            other.length = 0;
            other.data = nullptr;
        }

        void push_back(const T& value){
            if (length == capacity){
                resize();
            }
            std::construct_at(&data[length], value);
            length++;
        }
        void push_back(T&& value){
            if (length == capacity){
                resize();
            }
            std::construct_at(&data[length], std::move(value));
            length++;
        }

        void pop_back(){
            if (length == 0){
                throw std::out_of_range("Error: Cannot pop_back when Vector is Empty");
            }
            length--;
            std::destroy_at(&data[length]);
        }

        const T& operator[](size_t idx) const noexcept {
            return data[idx];
        }
        T& operator[](size_t idx) noexcept {
            return data[idx];
        }

        const T& at(size_t idx) const {
            if (idx >= length){
                throw std::out_of_range("Error: Index out of bounds");
            }
            return data[idx];
        }
        T& at(size_t idx){
            if (idx >= length){
                throw std::out_of_range("Error: Index out of bounds");
            }
            return data[idx];
        }

    private:
        T* data;
        size_t capacity;
        size_t length;
        std::allocator<T> allocator;

        void resize(){
            size_t new_capacity = (capacity == 0) ? 1 : capacity * 2;
            T* new_data = allocator.allocate(new_capacity);
            std::uninitialized_move(data, data + length, new_data);

            std::destroy(data, data + length);
            allocator.deallocate(data, capacity);

            data = new_data;
            capacity = new_capacity;
        }
        void clear() noexcept {
            for (size_t i = 0; i < length; i++){
                std::destroy_at(&data[i]);
            }
            length = 0;
        }
    };
}