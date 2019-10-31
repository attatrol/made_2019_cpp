#ifndef LINEARALLOCATOR_H
#define LINEARALLOCATOR_H

#include <cstring>

/* Linear allocator */
class LinearAllocator
{
private:
    const std::size_t m_maxSize; /* size of managed memory in bytes */
    std::size_t m_position;      /* current index of first unallocated byte */
    char* const m_heap;          /* dynamic array with bytes */
public:
    /* Default ctor */
    LinearAllocator(std::size_t maxSize);
    /* Dtor */
    ~LinearAllocator() { operator delete(m_heap); }
    /* Allocate memory chunk */
    char* alloc(std::size_t size);
    /* "Free" allocated memory so that all memory can be allocated once again */
    void reset() { m_position = 0; }
    /* Size of managed memory */
    std::size_t getMaxSize() { return m_maxSize; }
    /* Number of free aviable memory */
    std::size_t getResidue() { return m_maxSize - m_position; }
};
#endif // LINEARALLOCATOR_H
