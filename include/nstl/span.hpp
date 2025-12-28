#include <cstddef>
#include <stdexcept>
#include <limits>
#include <iterator>
#include <nstl/vector.hpp>

namespace nstl {
    inline constexpr size_t dynamic_extent = std::numeric_limits<size_t>::max();

    template <typename T, size_t Extent = dynamic_extent>
    class span {
    public:
        constexpr span() noexcept : _ptr(nullptr), _size(0) {}
        explicit constexpr span(T* ptr, size_t count) noexcept : _ptr(ptr), _size(count) {}
        constexpr span(const span&) noexcept = default;
        constexpr span(span&&) noexcept = default;

        template <size_t N>
        explicit constexpr span(T (&arr)[N]) noexcept : span(arr, N) {}
        template <typename Alloc>
        constexpr span(nstl::vector<T, Alloc>& vec) noexcept : span(vec.empty() ? nullptr : &vec[0], vec.size()) {}
        template <typename Alloc>
        constexpr span(const nstl::vector<T, Alloc>& vec) noexcept : span(vec.empty() ? nullptr : &vec[0], vec.size()) {}

        constexpr span& operator=(const span&) noexcept = default;
        constexpr span& operator=(span&&) noexcept = default;

        constexpr size_t size() const noexcept {return _size;}
        constexpr size_t size_bytes() const noexcept {return _size * sizeof(T);}
        constexpr T* data() const noexcept {return _ptr;}
        constexpr bool empty() const noexcept {return _size == 0;}
        constexpr T& operator[](size_t idx) const noexcept { return _ptr[idx];}

        constexpr T& front() noexcept {return _ptr[0];}
        constexpr const T& front() const noexcept {return _ptr[0];}
        constexpr T& back() noexcept {return _ptr[_size - 1];}
        constexpr const T& back() const noexcept {return _ptr[_size - 1];}
        T& at(size_t idx) {
            if (idx >= _size){
                throw std::out_of_range("nstl::span::at index out of range");
            }
            return _ptr[idx];
        }
        const T& at(size_t idx) const {
            if (idx >= _size){
                throw std::out_of_range("nstl::span::at index out of range");
            }
            return _ptr[idx];
        }

        constexpr T* begin() noexcept { return _ptr; }
        constexpr const T* begin() const noexcept { return _ptr; }
        constexpr const T* cbegin() const noexcept { return _ptr; }

        constexpr T* end() noexcept { return _ptr + _size; }
        constexpr const T* end() const noexcept { return _ptr + _size; }
        constexpr const T* cend() const noexcept { return _ptr + _size; }

        constexpr std::reverse_iterator<T*> rbegin() noexcept {
            return std::reverse_iterator<T*>(end());
        }
        constexpr std::reverse_iterator<const T*> rbegin() const noexcept {
            return std::reverse_iterator<const T*>(end());
        }
        constexpr std::reverse_iterator<T*> rend() noexcept {
            return std::reverse_iterator<T*>(begin());
        }
        constexpr std::reverse_iterator<const T*> rend() const noexcept {
            return std::reverse_iterator<const T*>(begin());
        }

        constexpr span<T> first(size_t count) const noexcept {return span<T>(_ptr, count);}
        constexpr span<T> last(size_t count) const noexcept {return span<T>(_ptr + (_size - count), count);}
        constexpr span<T> subspan(size_t offset, size_t count = dynamic_extent) const noexcept {
            size_t avail = _size - offset;
            size_t len = (count == dynamic_extent) ? avail : count;
            return span<T>(_ptr + offset, len);
        }

    private:
        T* _ptr;
        size_t _size;
    };

    template <typename T, size_t Extent>
    requires (Extent != dynamic_extent)
    class span {
    public:
        constexpr span() noexcept : _ptr(nullptr) {}
        explicit constexpr span(T* ptr) noexcept : _ptr(ptr) {}
        constexpr span(const span&) noexcept = default;
        constexpr span(span&&) noexcept = default;

        explicit constexpr span(T (&arr)[Extent]) noexcept : span(arr) {}

        constexpr span& operator=(const span&) noexcept = default;
        constexpr span& operator=(span&&) noexcept = default;

        constexpr size_t size() const noexcept {return Extent;}
        constexpr size_t size_bytes() const noexcept {return Extent * sizeof(T);}
        constexpr T* data() const noexcept {return _ptr;}
        constexpr bool empty() const noexcept {return Extent == 0;}
        constexpr T& operator[](size_t idx) const noexcept { return _ptr[idx];}

        constexpr T& front() noexcept {return _ptr[0];}
        constexpr const T& front() const noexcept {return _ptr[0];}
        constexpr T& back() noexcept {return _ptr[Extent - 1];}
        constexpr const T& back() const noexcept {return _ptr[Extent - 1];}
        T& at(size_t idx) {
            if (idx >= Extent){
                throw std::out_of_range("nstl::span::at index out of range");
            }
            return _ptr[idx];
        }
        const T& at(size_t idx) const {
            if (idx >= Extent){
                throw std::out_of_range("nstl::span::at index out of range");
            }
            return _ptr[idx];
        }

        constexpr T* begin() noexcept { return _ptr; }
        constexpr const T* begin() const noexcept { return _ptr; }
        constexpr const T* cbegin() const noexcept { return _ptr; }

        constexpr T* end() noexcept { return _ptr + Extent; }
        constexpr const T* end() const noexcept { return _ptr + Extent; }
        constexpr const T* cend() const noexcept { return _ptr + Extent; }

        constexpr std::reverse_iterator<T*> rbegin() noexcept {
            return std::reverse_iterator<T*>(end());
        }
        constexpr std::reverse_iterator<const T*> rbegin() const noexcept {
            return std::reverse_iterator<const T*>(end());
        }
        constexpr std::reverse_iterator<T*> rend() noexcept {
            return std::reverse_iterator<T*>(begin());
        }
        constexpr std::reverse_iterator<const T*> rend() const noexcept {
            return std::reverse_iterator<const T*>(begin());
        }

        constexpr span<T> first(size_t count) const noexcept {return span<T>(_ptr, count);}
        constexpr span<T> last(size_t count) const noexcept {return span<T>(_ptr + (Extent - count), count);}
        constexpr span<T> subspan(size_t offset, size_t count = dynamic_extent) const noexcept {
            size_t avail = Extent - offset;
            size_t len = (count == dynamic_extent) ? avail : count;
            return span<T>(_ptr + offset, len);
        }

        template <size_t Count>
        constexpr span<T, Count> first() const noexcept {
            static_assert(Count <= Extent, "first<Count>: Count > Extent");
            return span<T, Count>(_ptr);
        }

        template <size_t Count>
        constexpr span<T, Count> last() const noexcept {
            static_assert(Count <= Extent, "last<Count>: Count > Extent");
            return span<T, Count>(_ptr + (Extent - Count));
        }

        template <size_t Offset, size_t Count = dynamic_extent>
        constexpr auto subspan() const noexcept {
            static_assert(Offset <= Extent, "subspan<Offset>: Offset > Extent");
            constexpr size_t avail = Extent - Offset;
            constexpr size_t new_extent = (Count == dynamic_extent ? avail : Count);
            static_assert(new_extent <= avail, "subspan<Offset, Count>: too large");

            return span<T, new_extent>(_ptr + Offset);
        }

    private:
        T* _ptr;
    };
}