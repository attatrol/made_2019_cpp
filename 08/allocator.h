#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstdlib>
#include <exception>
#include <limits>

template <class T>
class Allocator {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = std::size_t;
public:
    pointer allocate(size_type capacity)
    {
        pointer chunk = static_cast<pointer>(malloc(capacity * sizeof(value_type)));
        if (!chunk)
        {
            throw std::bad_alloc();
        }
        return chunk;
    }
    void deallocate(pointer ptr, size_type capacity)
    {
        free(ptr);
    }
    size_t max_size() const noexcept
    {
        return std::numeric_limits<size_type>::max();
    }
    template<class U, class... Args>
    void construct(U* p, Args&&... args)
    {
        new ((void *)p) U(std::forward<Args>(args)...);
    }
    template<class U>
    void destroy(U* p)
    {
        p->~U();
    }
};

template< class T1, class T2 >
constexpr bool operator==( const Allocator<T1>& lhs, const Allocator<T2>& rhs ) noexcept
{
    return std::addressof(lhs) == std::addressof(rhs);
}

#endif // ALLOCATOR_H
