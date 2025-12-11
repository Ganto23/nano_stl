#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>

namespace nstl {
    template<typename T>
    class vector{
    public:
        vector(size_t initial_capacity = 1){
            _capacity = initial_capacity;
            _length = 0;
            _data = _allocator.allocate(_capacity);
        }
        ~vector(){
            clear();
            if (_data){
                _allocator.deallocate(_data, _capacity);
            }
        }
        vector(const vector& other): _capacity(other._capacity), _length(other._length) {
            _data = _allocator.allocate(_capacity);
            std::uninitialized_copy(other._data, other._data + other._length, _data);
        }
        vector& operator=(vector other) {
            std::swap(_data, other._data);
            std::swap(_capacity, other._capacity);
            std::swap(_length, other._length);
            return *this;
        }

        vector(vector&& other) noexcept : _capacity(other._capacity), _length(other._length), _data(other._data) {
            other._capacity = 0;
            other._length = 0;
            other._data = nullptr;
        }

        void push_back(const T& value){
            if (_length == _capacity){
                size_t new_capacity = (_capacity == 0) ? 1 : _capacity * 2;
                resize(new_capacity);
            }
            std::construct_at(&_data[_length], value);
            _length++;
        }
        void push_back(T&& value){
            if (_length == _capacity){
                size_t new_capacity = (_capacity == 0) ? 1 : _capacity * 2;
                resize(new_capacity);
            }
            std::construct_at(&_data[_length], std::move(value));
            _length++;
        }

        void pop_back(){
            if (_length == 0){
                throw std::out_of_range("Error: Cannot pop_back when Vector is Empty");
            }
            _length--;
            std::destroy_at(&_data[_length]);
        }

        const T& operator[](size_t idx) const noexcept {
            return _data[idx];
        }
        T& operator[](size_t idx) noexcept {
            return _data[idx];
        }

        const T& at(size_t idx) const {
            if (idx >= _length){
                throw std::out_of_range("Error: Index out of bounds");
            }
            return _data[idx];
        }
        T& at(size_t idx){
            if (idx >= _length){
                throw std::out_of_range("Error: Index out of bounds");
            }
            return _data[idx];
        }

        size_t size(){
            return _length;
        }

        bool empty(){
            return _length == 0;
        }

        size_t capacity(){
            return _capacity;
        }

        void reserve(size_t new_capacity){
            if (new_capacity <= _capacity){
                return;
            }
            resize(new_capacity);
            return;
        }

        void shrink_to_fit(){
            //TODO
        }

    private:
        T* _data;
        size_t _capacity;
        size_t _length;
        std::allocator<T> _allocator;

        void resize(size_t new_capacity){
            T* new_data = _allocator.allocate(new_capacity);
            std::uninitialized_move(_data, _data + _length, new_data);

            std::destroy(_data, _data + _length);
            _allocator.deallocate(_data, _capacity);

            _data = new_data;
            _capacity = new_capacity;
        }
        void clear() noexcept {
            for (size_t i = 0; i < _length; i++){
                std::destroy_at(&_data[i]);
            }
            _length = 0;
        }
    };
}