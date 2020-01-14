#include <iostream>
#include <sstream>
#include <vector>

#include "matrix.h"

/*
 * Create simple matrices.
 */
Matrix createMatrix2x2_1()
{
    int* data = new int[4];
    for (std::size_t i = 0; i < 4; ++i)
    {
        data[i] = i;
    }
    Matrix result(data, 2, 2);
    delete[] data;
    return result;
}
Matrix createMatrix2x2_2()
{
    int* data = new int[4];
    for (std::size_t i = 0; i < 4; ++i)
    {
        data[i] = i + 1;
    }
    Matrix result(data, 2, 2);
    delete[] data;
    return result;
}
Matrix createMatrix2x2_3()
{
    int* data = new int[4];
    for (std::size_t i = 0; i < 4; ++i)
    {
        data[i] = i;
        data[i] *= -57;
    }
    Matrix result(data, 2, 2);
    delete[] data;
    return result;
}
Matrix createMatrix2x2_zero()
{
    int* data = new int[4];
    for (std::size_t i = 0; i < 4; ++i)
    {
        data[i] = 0;
    }
    Matrix result(data, 2, 2);
    delete[] data;
    return result;
}
Matrix createMatrix4x5()
{
    int* data = new int[20];
    for (std::size_t i = 0; i < 20; ++i)
    {
        data[i] = i;
    }
    Matrix result(data, 4, 5);
    delete[] data;
    return result;
}
std::vector<Matrix> createMatrices()
{
    return { createMatrix2x2_1(), createMatrix2x2_2(), createMatrix2x2_3(), createMatrix2x2_zero(), createMatrix4x5() };
}

/* Test creation matrices of illegal size */
bool testBadInit() {
    try {
        Matrix result(nullptr, 0, 5);
        return false;
    } catch (const std::out_of_range& e) {
        return true;
    }
    try {
        Matrix result(nullptr, 100, 0);
        return false;
    } catch (const std::out_of_range& e) {
    }
    try {
        Matrix result(nullptr, 0, 0);
        return false;
    } catch (const std::out_of_range& e) {
    }
    return true;
}

/* Test comparison of matrices */
bool testComparison() {
    auto v1 = createMatrices();
    auto v2 = createMatrices();

    for(std::size_t i = 0; i != v1.size(); ++i) {
        for(std::size_t j = 0; j != v2.size(); ++j) {
            if (!(i == j ? v1[i] == v1[j] && v1[i] == v2[j] : v1[i] != v1[j] && v1[i] != v2[j]))
            {
                return false;
            }
        }
    }
    return true;
}

/* Test matrix assignment */
bool testAssignment() {
    auto v = createMatrices();

   for(auto const& value: v) {
       Matrix matrix = value;
       if (matrix != value)
       {
           return false;
       }
   }
   return true;
}

/* Test access to matrix element by index */
bool testValueAccess() {
    Matrix m = createMatrix2x2_1();
    Matrix target = createMatrix2x2_3();

   m[0][0] = target[0][0];
   m[0][1] = target[0][1];
   m[1][0] = target[1][0];
   m[1][1] = target[1][1];

   try {
       m[3];
       m[0][7];
       return false;
   } catch (const std::out_of_range& e) {
   }

   return m == target;
}

/* Test matrix multiplication on scalar */
bool testScalarMultiplication() {
    Matrix m = createMatrix2x2_1();
    Matrix target = createMatrix2x2_3();
    m *= -57;
    if (m != target)
    {
        return false;
    }

    m = createMatrix2x2_1();
    Matrix zero = createMatrix2x2_zero();
    m *= 0;
    if (m != zero)
    {
        return false;
    }

    try {
        m = createMatrix2x2_1();
        m *= std::numeric_limits<int>::max();
        return false;
    } catch (const std::out_of_range& e) {
    }

    try {
        m = createMatrix2x2_1();
        m *= std::numeric_limits<int>::min();
        return false;
    } catch (const std::out_of_range& e) {
    }

   return true;
}

/** Test suit */
void runTests() {
    std::cout << "Test run started." << std::endl;
    if (!testBadInit())
        std::cout << "Matrix with zero sizes was created" << std::endl;
    if (!testComparison())
        std::cout << "Comparison tests failed" << std::endl;
    if (!testAssignment())
        std::cout << "Assignment tests failed" << std::endl;
    if (!testValueAccess())
        std::cout << "Value access tests failed" << std::endl;
    if (!testScalarMultiplication())
        std::cout << "Multiplication tests failed" << std::endl;
    std::cout << "Test run completed." << std::endl;
}

/** Program entry point */
int main(void) {
    runTests();
}
