#include <iostream>

#include <limits>
#include <sstream>

#include "linearallocator.h"

/*
 * Ensure we will have no problems with alignment.
 * For x64 this test is redundant.
 */
bool testNoAlignmentNeeded()
{
    return sizeof(char) == 1;
}

/*
 * Test for a bad_alloc throws.
 * Also figure out what is the safe size of memory to be allocated in other tests
*/
bool testBadAlloc(std::size_t& safeMaxSize)
{
    safeMaxSize = std::numeric_limits<std::size_t>::max();
    while (safeMaxSize)
    {
        try
        {
            LinearAllocator la(safeMaxSize);
        }
        catch (const std::bad_alloc&)
        {
            safeMaxSize >>= 1;
            if (safeMaxSize <= 65535)
            {
                return false; // the standart says SIZE_MAX must be at least 65535
            }
            continue;
        }
        break;
    }
    return true;
}

bool testAfterCreationInvariants(LinearAllocator& la, const std::size_t maxSize)
{
    return la.getMaxSize() == maxSize && la.getResidue() == maxSize;
}

/* Test linear exhaustive memory allocation */
bool testExhastiveAlloc(LinearAllocator& la)
{
    for (std::size_t i = 1; i <= la.getMaxSize(); ++i)
    {
        std::size_t chunkCount = la.getMaxSize() / i;
        std::size_t residualChunkSize = la.getMaxSize() % i;

        la.reset();
        if (la.getMaxSize() != la.getResidue())
        {
            return false;
        }
        char* prev = la.alloc(i);
        for (std::size_t j = 1; j < chunkCount; ++j)
        {
            char* curr = la.alloc(i);
            if (prev == nullptr || curr == nullptr || curr - i != prev)
            {
                return false;
            }
            prev = curr;
        }
        if (residualChunkSize)
        {
            char* curr = la.alloc(residualChunkSize);
            if (prev == nullptr || curr == nullptr || curr - i != prev)
            {
                return false;
            }
        }
        char* curr = la.alloc(1);
        if (curr != nullptr)
        {
            return false;
        }
    }
    return true;
}

/* Test impossibility of allocation of chink greater than it should be possible */
bool testOverflowAlloc(LinearAllocator& la)
{
    la.reset();
    if (la.getMaxSize() != la.getResidue())
    {
        return false;
    }
    for (std::size_t i = la.getMaxSize() + 1; i == std::numeric_limits<std::size_t>::max(); i = (i << 1) + 1)
    {
        if (la.alloc(i) == nullptr)
        {
            return false;
        }
    }
    return true;
}

/* Test impossibility of allocation of zero-sized chink */
bool testZeroAlloc(LinearAllocator& la)
{
    return la.alloc(0) == nullptr;
}


/* Test addition as utility arithmetic operation */
void testLinearAllocator(std::size_t maxSize) {
    LinearAllocator la(maxSize);
    if (!testAfterCreationInvariants(la, maxSize))
    {
       std::cout << "Failed to fullfill basic invariants after creation. MaxSize = " << maxSize << std::endl;
    }
    if (!testExhastiveAlloc(la))
    {
       std::cout << "Failed to allocate all memory as continuous sequence of chunks. MaxSize = " << maxSize << std::endl;
    }
    if (!testOverflowAlloc(la))
    {
       std::cout << "Failed to return nullptr on too large chunk allocation. MaxSize = " << maxSize << std::endl;
    }
    if (!testZeroAlloc(la))
    {
       std::cout << "Failed to return nullptr on zero sized chunk allocation. MaxSize = " << maxSize << std::endl;
    }
}

const std::size_t TEST_MAX_SIZE = 256;

/* Test suit */
void runTests() {
    if (!testNoAlignmentNeeded())
    {
        std::cout << "sizeof(char) != 1" << std::endl;
    }
    std::size_t safeMaxSize;
    if (!testBadAlloc(safeMaxSize))
    {
        std::cout << "Something is very wrong. Failed to allocate memory of expected size" << std::endl;
    }
    std::size_t testMaxSize = TEST_MAX_SIZE < safeMaxSize ? TEST_MAX_SIZE : safeMaxSize;
    for (std::size_t i = 0; i <= testMaxSize; ++i)
    {
        testLinearAllocator(i);
    }
    std::cout << "Test run completed." << std::endl;
}

/* Program entry point */
int main(void) {
    runTests();
}
