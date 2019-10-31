#include "linearallocator.h"

LinearAllocator::LinearAllocator(std::size_t maxSize): m_maxSize(maxSize), m_position(0), m_heap(static_cast<char*>(operator new(maxSize)))
{
}

char *LinearAllocator::alloc(std::size_t size)
{
    std::size_t newPosition = m_position + size;
    if (newPosition < m_position || newPosition > m_maxSize || !size) {
        return nullptr;
    }
    std::size_t m_addrOffset = m_position;
    m_position = newPosition;
    return m_heap + m_addrOffset;
}
