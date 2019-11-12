#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <cstring>
#include <stdexcept>

/* Integer matrix */
class Matrix
{
private:
    /* Matrix row proxy class */
    class Row {
        friend class Matrix;
    private:
        int* m_data;                         /* link to the row data array */
        std::size_t& m_size;                 /* matrix row length */
        Row(std::size_t& size): m_size(size)
        {
        }
        /* Set actual pointer to row data */
        void setData(int* data)
        {
            m_data = data;
        }
    public:
        /*
         * Standard creation and copy are forbidden.
         * A lot of problems with coherency are avoided this way.
         */
        const Row& operator=(const Row&)
        {
            return *this;
        }
        Row& operator=(Row&&) = delete;
        Row(const Row&) = delete;
        Row(Row&&) = delete;
        /* Access index operator */
        int& operator[](std::size_t idx)
        {
            if (idx >= m_size)
            {
                throw std::out_of_range("Matrix column index out of range");
            }
            return m_data[idx];
        }
        std::size_t getSize() const
        {
            return m_size;
        }
    };

    std::size_t m_rowCount; /* row count */
    std::size_t m_colCount; /* column count */
    int* m_data;            /* flat array that contains the matrix values placed row after row */
    Row m_row;              /* row access proxy */
private:
    /* Get matrix value count */
    std::size_t getSize() const
    {
        return m_rowCount * m_colCount;
    }
    /* Check if ths matrix is equal to another matrix */
    bool equals(const Matrix& other) const;
    /* Multiply matrix on scalar */
    void multiply(int factor);
public:
    /* Default ctor */
    Matrix(int* data, std::size_t rowCount, std::size_t colCount);
    /* Copy ctor */
    Matrix(const Matrix& other);
    /* Copy assignment operator */
    Matrix& operator=(const Matrix& other);
    ~Matrix()
    {
        delete[] m_data;
    }
    /* Get row by index */
    Row& operator[](std::size_t idx)
    {
        if (idx >= m_rowCount)
        {
            throw std::out_of_range("Matrix row index out of range");
        }
        m_row.setData(m_data + idx * m_colCount);
        return m_row;
    }
    /* Equals to operator */
    bool operator ==(const Matrix& other) const
    {
        return equals(other);
    }
    /* Inequal to operator */
    bool operator !=(const Matrix& other) const
    {
        return !equals(other);
    }
    /* Scalar multiplication operator */
    Matrix& operator *=(int factor)
    {
        multiply(factor);
        return *this;
    }
};

#endif // MATRIX_H
