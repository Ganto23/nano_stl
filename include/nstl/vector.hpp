#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <utility>
#include <cstring>
#include <type_traits>
#include <algorithm>
#include <concepts>

namespace nstl {
    template<typename T>
    class vector{
    public:
        using iterator = T*;
        using const_iterator = const T*;
        //using reference = T&;
        //using const_reference = const T&;
        
        constexpr vector() noexcept : _capacity(0), _length(0), _data(nullptr) {}
        constexpr explicit vector(size_t initial_capacity) {
            if (initial_capacity > 0) [[likely]] {
                _capacity = initial_capacity;
                _length = 0;
                _data = _allocator.allocate(_capacity);
            } else {
                _capacity = 0;
                _length = 0;
                _data = nullptr;
            }
        }
        constexpr ~vector(){
            clear();
            if (_data){
                _allocator.deallocate(_data, _capacity);
            }
        }
        constexpr vector(const vector& other): _capacity(other._capacity), _length(other._length) {
            _data = _allocator.allocate(_capacity);
            std::copy(other._data, other._data + other._length, _data);
        }
        constexpr vector(vector&& other) noexcept : _capacity(other._capacity), _length(other._length), _data(other._data) {
            other._capacity = 0;
            other._length = 0;
            other._data = nullptr;
        }
        constexpr vector& operator=(const vector& other) {
            if (this == &other) return *this;
            
            if (other._length > _capacity) {
                clear();
                if (_data) _allocator.deallocate(_data, _capacity);
                _capacity = other._length;
                _data = _allocator.allocate(_capacity);
            } else {
                clear();
            }

            _length = other._length;
            if constexpr (std::is_trivially_copyable_v<T>) {
                 if (std::is_constant_evaluated()) {
                     std::copy(other._data, other._data + _length, _data);
                 } else {
                     if (_length > 0) std::memcpy(_data, other._data, _length * sizeof(T));
                 }
            } else {
                std::uninitialized_copy(other._data, other._data + _length, _data);
            }
            return *this;
        }
        constexpr vector& operator=(vector&& other) noexcept {
            if (this != &other){
                clear();
                if (_data) _allocator.deallocate(_data, _capacity);
                _data = other._data;
                _length = other._length;
                _capacity = other._capacity;
                other._data = nullptr;
                other._length = 0;
                other._capacity = 0;
            }
            return *this;
        }

        constexpr void push_back(const T& value){
            if (_length == _capacity) {
                size_t new_capacity = _capacity ? _capacity * 2 : 8;
                resize(new_capacity);
            }
            std::construct_at(&_data[_length], value);
            _length++;
        }
        constexpr void push_back(T&& value) noexcept {
            if (_length == _capacity) {
                size_t new_capacity = _capacity ? _capacity * 2 : 8;
                resize(new_capacity);
            }
            std::construct_at(&_data[_length], std::move(value));
            _length++;
        }

        template <typename... Args>
        constexpr T& emplace_back(Args&&... args) {
            if (_length == _capacity) [[unlikely]] {
                return emplace_back_slow(std::forward<Args>(args)...);
            }
            T* ptr = std::construct_at(&_data[_length], std::forward<Args>(args)...);
            _length++;
            return *ptr;
        }

        constexpr void pop_back(){
            if (_length == 0) [[unlikely]] {
                throw std::out_of_range("Error: Cannot pop_back when Vector is Empty");
            }
            _length--;
            std::destroy_at(&_data[_length]);
        }

        constexpr const T& operator[](size_t idx) const noexcept {
            return _data[idx];
        }
        T& operator[](size_t idx) noexcept {
            return _data[idx];
        }

        constexpr const T& at(size_t idx) const {
            if (idx >= _length) [[unlikely]] {
                throw std::out_of_range("Error: Index out of bounds");
            }
            return _data[idx];
        }
        constexpr T& at(size_t idx){
            if (idx >= _length) [[unlikely]] {
                throw std::out_of_range("Error: Index out of bounds");
            }
            return _data[idx];
        }

        constexpr size_t size() const noexcept {return _length;}
        constexpr bool empty() const noexcept {return _length == 0;}
        constexpr size_t capacity() const noexcept {return _capacity;}

        constexpr void reserve(size_t new_capacity){
            if (new_capacity <= _capacity){
                return;
            }
            resize(new_capacity);
            return;
        }

        constexpr void shrink_to_fit(){
            if (_length == _capacity) [[unlikely]] {
                return;
            }
            resize(_length);
            return;
        }

        constexpr iterator begin() noexcept { return _data; }
        constexpr iterator end() noexcept { return _data + _length; }
        constexpr const_iterator begin() const noexcept { return _data; }
        constexpr const_iterator end() const noexcept { return _data + _length; }
        constexpr const_iterator cbegin() const noexcept { return _data; }
        constexpr const_iterator cend() const noexcept { return _data + _length; }

    private:
        [[no_unique_address]] std::allocator<T> _allocator;
        size_t _capacity;
        size_t _length;
        T* _data;
        
        constexpr void resize(size_t new_capacity) noexcept {
            T* new_data = _allocator.allocate(new_capacity);

            if constexpr (std::is_trivially_copyable_v<T>) {
                if (std::is_constant_evaluated()){
                    std::copy(_data, _data + _length, new_data);
                } else {
                    if (_length > 0) std::memcpy(new_data, _data, _length * sizeof(T));
                }
            } else {
                std::uninitialized_move(_data, _data + _length, new_data);
                std::destroy(_data, _data + _length);
            }

            if (_data) [[likely]] {
                _allocator.deallocate(_data, _capacity);
            }

            _data = new_data;
            _capacity = new_capacity;
        }

        constexpr void clear() noexcept {
            if constexpr (!std::is_trivially_destructible_v<T>){
                for (size_t i = 0; i < _length; i++){
                    std::destroy_at(&_data[i]);
                }
            }
            _length = 0;
        }

        template <typename... Args>
        //__attribute__((noinline, cold))
        constexpr T& emplace_back_slow(Args&&... args) {
            size_t new_capacity = _capacity ? _capacity * 2 : 8;
            T* new_data = _allocator.allocate(new_capacity);

            T* new_element = std::construct_at(&new_data[_length], std::forward<Args>(args)...);

            if constexpr (std::is_trivially_copyable_v<T>) {
                if (std::is_constant_evaluated()){
                    std::copy(_data, _data + _length, new_data);
                } else {
                    if (_length > 0) std::memcpy(new_data, _data, _length * sizeof(T));
                }
            } else {
                for (size_t i = 0; i < _length; ++i) {
                    std::construct_at(&new_data[i], std::move(_data[i]));
                    std::destroy_at(&_data[i]);
                }
            }

            if (_data) _allocator.deallocate(_data, _capacity);

            _data = new_data;
            _capacity = new_capacity;
            _length++;
            return *new_element;
        }
    };
}