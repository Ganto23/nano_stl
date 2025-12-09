#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>

namespace nstl {
    template<typename T>
    class vector{
    public:
        vector(size_t initial_capacity){
            vector = std::make_unique<T[]>(initial_capacity);
            capacity = initial_capacity;
            length = 0;
        }
        vector(const vector&) = delete;
        vector& operator=(const vector&) = delete;
        vector(vector&&) = default;
        vector& operator=(vector&&) = default;

        void push_back(T value){
            if (length >= capacity){
                resize();
            }
            vector[length] = value;
            length++;
        }
        T pop_back(){
            if (length == 0){
                throw std::out_of_range("Error: Cannot pop_back when Vector is Empty");
            }
            length--;
            return vector[length];
        }
        void insert(T value, size_t idx){
            if (idx >= length || idx < 0){
                throw std::out_of_range("Error: Index out of bounds");
            }
            if (length >= capacity){
                resize();
            }
            for (size_t i = length; i > static_cast<size_t>(idx); --i){
                vector[i] = vector[i - 1];
            }
            vector[static_cast<size_t>(idx)] = value;
            length++;
        }
        T operator[](size_t idx){
            if (idx >= length || idx < 0){
                throw std::out_of_range("Error: Index out of bounds");
            }
            return vector[idx];
        }

    private:
        std::unique_ptr<T[]> vector;
        size_t capacity;
        size_t length;

        void resize(){
            std::unique_ptr<T[]> new_vector;
            capacity = capacity ? capacity*2 : 1;
            new_vector = std::make_unique<T[]>(capacity);
            for (size_t i = 0; i < length; i++){
                new_vector[i] = vector[i];
            }
            vector = std::move(new_vector);
        }
    };
}