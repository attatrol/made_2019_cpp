#include "matrix.h"

Matrix::Matrix(int* data, std::size_t rowCount, std::size_t colCount): m_rowCount(rowCount), m_colCount(colCount), m_data(nullptr)
{
    std::size_t size = getSize();
    if (!size)
    {
        if (colCount || rowCount)
        {
            throw std::out_of_range("Zero aand nonzero matrix sizes mixed");
        }
    }
    else
    {
        m_data = new int[size];
        std::copy(data, data + size, m_data);
    }
}

Matrix::Matrix(): m_rowCount(0), m_colCount(0), m_data(nullptr)
{
}

Matrix::Matrix(const Matrix& other): m_rowCount(other.m_rowCount), m_colCount(other.m_colCount), m_data(nullptr)
{
    std::size_t size = getSize();
    m_data = new int[size];
    std::copy(other.m_data, other.m_data + size, m_data);
}

Matrix& Matrix::operator=(const Matrix& other)
{
    if(this != &other) {
        std::size_t size = other.getSize();
        if (getSize() != size)
        {
            delete[] m_data;
            m_data = new int[size];
        }
        std::copy(other.m_data, other.m_data + size, m_data);
        m_rowCount = other.m_rowCount;
        m_colCount = other.m_colCount;
    }
    return *this;
}

bool Matrix::equals(const Matrix& other) const
{
    if (m_rowCount != other.m_rowCount || m_colCount != other.m_colCount)
    {
        return false;
    }
    std::size_t const size = getSize();
    const int* otherData = other.m_data;
    for (std::size_t i = 0; i < size; ++i)
    {
        if (otherData[i] != m_data[i])
        {
            return false;
        }
    }
    return true;
}

/* Test if multiplicatio n doesn't cause overflow */
inline void testMul(int lhs, int rhs) {
    if (lhs > 0 && rhs > 0 && lhs > std::numeric_limits<int>::max() / rhs ||
        lhs < 0 && rhs > 0 && lhs < std::numeric_limits<int>::min() / rhs ||
        lhs > 0 && rhs < 0 && rhs < std::numeric_limits<int>::min() / lhs ||
        lhs < 0 && rhs < 0 && (lhs < std::numeric_limits<int>::min() || rhs < std::numeric_limits<int>::min() || -lhs > std::numeric_limits<int>::max() / -rhs)) {
        throw std::out_of_range("Matrix scalar multiplication overflow");
    }
}

void Matrix::multiply(int factor)
{
    std::size_t size = getSize();
    if (!factor)
    {
        std::memset(m_data, 0, size * sizeof(int));
        return;
    }

    /*
     * It is cheaper to find extremi of the matrix values and test them against overflow
     * than to check for overflow on every multiplication.
     * Also this makes this mutation rollbackable.
     */
    const auto minMax = std::minmax_element(m_data, m_data + size);
    testMul(*minMax.first, factor);
    testMul(*minMax.second, factor);

    for (std::size_t i = 1; i < size; ++i)
    {
        m_data[i] *= factor;
    }
}

Matrix::Row Matrix::operator[](std::size_t idx)
{
    if (idx >= m_rowCount)
    {
        throw std::out_of_range("Matrix row index out of range");
    }
    return Row(m_data + idx * m_colCount, m_rowCount);
}

int& Matrix::Row::operator[](std::size_t idx)
{
    if (idx >= m_size)
    {
        throw std::out_of_range("Matrix column index out of range");
    }
    return m_data[idx];
}
