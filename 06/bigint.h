#ifndef BIGINT_H
#define BIGINT_H

#include <assert.h>
#include <cstdlib>
#include <limits>
#include <iostream>
#include <type_traits>

#define CELL_SIZE(T) (2 * sizeof(T) / sizeof(BigInt::cell_t) == 0 ? 1 : 2 * sizeof(T) / sizeof(BigInt::cell_t))
#define INTEGER_SHIFT(T, value) (sizeof(T) * 8 < CELL_BIT_COUNT ? ; : value >>= CELL_BIT_COUNT;)

/** Calculate number of decimal digits that fits one cell */
constexpr std::size_t calcDecimalDigits(const unsigned long long max)
{
    return max < 10 ? 1 : calcDecimalDigits(max / 10) + 1;
}

/** Calculate max power of 10 that fits one cell */
constexpr std::size_t calcMaxDecimal(const unsigned long long max)
{
    return max < 10 ? 1 : calcMaxDecimal(max / 10) * 10;
}

/** Integer of arbitraty size, supports addition, substraction and division */
class BigInt
{
public:
    using cell_t = unsigned int; /** cell type, may be any integer  */
    const static std::size_t CELL_BIT_COUNT = sizeof(cell_t) * 8 / 2; /* Number of bits in the used part of the cell. 
                                                                       * Cell is normalized if only the low half of it contains digits.
                                                                       * Higher part contains zero.
                                                                       * While cell may become denormalized during calculations,
                                                                       * at the end of mutation all cells must be normalised.
                                                                       */
    const static cell_t CELL_MASK = std::numeric_limits<cell_t>::max() >> CELL_BIT_COUNT; /* Mask for the normalized part of cell */
    constexpr static std::size_t DECIMAL_CELL_DIGITS = calcDecimalDigits(CELL_MASK) - 1; /* Max number of decimal digits that a normalized cell contains */
    constexpr static std::size_t TO_DECIMAL_CELL_DIVISOR = calcMaxDecimal(CELL_MASK); /* Max power of 10 that a normalized cell contains */
private:
    cell_t* m_cells;        /* cells, store the unsigned integer value */
    std::size_t m_capacity; /* size of m_cells array */
    std::size_t m_size;     /* number of used elements of m_cells, invariant m_size <= m_capacity must be mantained */
    bool m_isPositive;      /* the value sign */

    /* 
     * Copy ctor that can allocate more cells then the other instance has 
     * if it is obvious that subsequent actions would lead to the growth of used cell number
     */
    BigInt(const BigInt& other, std::size_t capacity);
    /* Verbose private ctor */
    BigInt(cell_t* cells, std::size_t capacity, std::size_t size, bool isPositive);
    
    ///////////////////////////////////////////////////////////
    /// Low level routines.
    /// They perform bitwise mutation of the object.
    /// However they should not modify m_isPositive flag.
    /// Most of them are not universal and their restrictions described by asserts.
    ///////////////////////////////////////////////////////////
    
    /* Соmpares the cells */
    int compareCells(const cell_t* cells, const std::size_t size) const;
    /* Compare to integer */
    template<typename T> int compareInteger(T value);
    /* Expand size with carried digits */
    void carry(cell_t carryValue);
    /* Expand current m_cells array capacity */
    void expandCapacity(std::size_t capacity);
    /* 
     * Decrement size if there are any zeroed senior cells.
     * After a single value change there must be no zeroed serior cells, so this must be called after substraction and multiplication.
     */
    void shrinkSize();
    /* Basic addition of another cells to the current ones */
    void addCells(const cell_t* cells, const std::size_t size);
    /* Adds self value to the cells, essentualy this means  */
    void addSelf();
    /* Addition of some immutable cells to the current ones */
    void addCellsCopy(const cell_t* cells, const std::size_t size);
    /* Addition of rvalue reference, its state may be used */
    void addCellsSwap(BigInt&& value);
    /* Increment cells */
    void incCells();
    /* Basic substraction of another cells from the current ones */
    void subCells(const cell_t* cells, const std::size_t size);
    /* Substraction of some immutable cells to the current ones */
    void subCellsCopy(const cell_t* cells, const std::size_t size);
    /* Substraction of rvalue reference, its state may be used */
    void subCellsSwap(BigInt&& value);
    /* Decrement cells */
    void decCells();
    /* Adds a denormalized single cell value to a specific place in the product */
    static void mulCellAdd(const cell_t value, const std::size_t pos, cell_t *product, const std::size_t size);
    /* Basic cell multiplication */
    static void mulCells(const cell_t* factor1, std::size_t size1, const cell_t* factor2, std::size_t size2, cell_t* product, std::size_t size);
    /* 
     * Divides the cells by the single cell divisor.
     * Puts the result in the provided array, doesn't nutate the object
     */
    static cell_t divModShort(cell_t* divident, std::size_t& size, const std::size_t divisor);
    /* Translates the current value into a decimal string */
    char* toDecimal() const;
    
    /////////////////////////////////////////
    /// Arithmetics methods.
    /// This methods are called by operators.
    /////////////////////////////////////////
    
    /* Compare to another BigInt */
    int compare(const BigInt& other) const;
    /* Add const value */
    void add(const BigInt& value);
    /* Add rvalue reference */
    void add(BigInt&& value);
    /* Substract const */
    void sub(const BigInt& value);
    /* Substract rvalue reference */
    void sub(BigInt&& value);
    /* Multiply const value */
    void mul(const BigInt& value);
    /* Multiply rvalue reference */
    void mul(BigInt&& value);
public:
    /* Ctor of zeroed inctance */
    BigInt();
    /* Ctor, creates BigInt by an integer value of any type */
    template <typename T, typename = std::enable_if_t<true == std::is_integral<T>::value>>
    BigInt(T value): m_cells(new cell_t[CELL_SIZE(T)]), m_capacity(CELL_SIZE(T)), m_size(CELL_SIZE(T)), m_isPositive(value >= 0)
    {
        // static_assert(std::is_integral<T>::value, "Integer argument required");
        if (!value)
        {
            m_size = 0;
            return;
        }
        bool increment = value == std::numeric_limits<T>::min();
        if (increment)
        {
            value = std::numeric_limits<T>::max();
        }
        else if (!m_isPositive)
        {
            value = -value;
        }
        m_cells[0] = value & CELL_MASK;
        for (std::size_t i = 1; i < CELL_SIZE(T); ++i)
        {
            // no UB despite warning : if CELL_BIT_COUNT >= sizeof(T) then the loop body will never be executed
            value >>= CELL_BIT_COUNT;  
            m_cells[i] = value & CELL_MASK;
        }
        if (increment)
        {
            incCells();
        }
        shrinkSize();
    }
    /* Copy ctor */
    BigInt(const BigInt& other);
    /* Copy assignment operator */
    BigInt& operator=(const BigInt& other);
    /* Move ctor */
    BigInt(BigInt&& other) noexcept;
    /* Move assignment operator */
    BigInt& operator=(BigInt&& other) noexcept;
    /* Dtor */
    ~BigInt()
    {
        delete[] m_cells;
    }
    /* 
     * Check if positive.
     * Zeroed instance may be either positive or negative,
     * this doesn't make any difference for the value.
     */
    bool isPositive() const
    {
        return m_isPositive;
    }
    /* Check if zero */
    bool isZero() const
    {
        return m_size == 0;
    }
    /* Ctor of zeroed inctance */
    void setZero()
    {
        m_size = 0;
    }

    /////////////////////////////////////////
    /// Operators.
    /////////////////////////////////////////

    inline bool operator<(const BigInt& rhs) const
    {
        return compare(rhs) == -1;
    }
    inline bool operator>(const BigInt& rhs) const
    {
        return compare(rhs) == 1;
    }
    inline bool operator<=(const BigInt& rhs) const
    {
        return compare(rhs) != 1;
    }
    inline bool operator>=(const BigInt& rhs) const
    {
        return compare(rhs) != -1;
    }
    inline bool operator==(const BigInt& rhs) const
    {
        return compare(rhs) == 0;
    }
    inline bool operator!=(const BigInt& rhs) const
    {
        return compare(rhs) != 0;
    }
    BigInt operator-() const & {
        BigInt result = BigInt(*this);
        result.m_isPositive = !result.m_isPositive;
        return result;
    }
    BigInt operator-() && {
        m_isPositive = !m_isPositive;
        return std::move(*this);
    }
    BigInt& operator++();
    BigInt& operator--();
    BigInt operator++(int);
    BigInt operator--(int);
    BigInt& operator+=(const BigInt& rhs);
    BigInt& operator+=(BigInt&& rhs);
    BigInt& operator-=(const BigInt& rhs);
    BigInt& operator-=(BigInt&& rhs);
    BigInt& operator*=(const BigInt& rhs);
    BigInt& operator*=(BigInt&& rhs);
    BigInt operator+(const BigInt& rhs) const &;
    BigInt operator+(const BigInt& rhs) &&;
    BigInt operator+(BigInt&& rhs) const &;
    BigInt operator+(BigInt&& rhs) &&;
    BigInt operator-(const BigInt& rhs) const &;
    BigInt operator-(const BigInt& rhs) &&;
    BigInt operator-(BigInt&& rhs) const &;
    BigInt operator-(BigInt&& rhs) &&;
    BigInt operator*(const BigInt& rhs) const &;
    BigInt operator*(const BigInt& rhs) &&;
    BigInt operator*(BigInt&& rhs) const &;
    BigInt operator*(BigInt&& rhs) &&;
    friend std::ostream& operator<<(std::ostream& out, const BigInt& value)
    {
        char* str = value.toDecimal();
        out << str;
        delete[] str;
        return out;
    }
};

template <typename T1, typename = std::enable_if_t<true == std::is_integral<T1>::value>,
          typename T2, class = std::enable_if_t<std::is_same<BigInt, std::decay_t<T2>>{}>>
BigInt operator+(T1 lhs, T2&& rhs)
{
    return BigInt(lhs) + std::forward<T2>(rhs);
}

template <typename T1, typename = std::enable_if_t<true == std::is_integral<T1>::value>,
          typename T2, class = std::enable_if_t<std::is_same<BigInt, std::decay_t<T2>>{}>>
BigInt operator-(T1 lhs, T2&& rhs)
{
    return BigInt(lhs) - std::forward<T2>(rhs);
}

template <typename T1, typename = std::enable_if_t<true == std::is_integral<T1>::value>,
          typename T2, class = std::enable_if_t<std::is_same<BigInt, std::decay_t<T2>>{}>>
BigInt operator*(T1 lhs, T2&& rhs)
{
    return BigInt(lhs) * std::forward<T2>(rhs);
}

template <typename T, typename = std::enable_if_t<true == std::is_integral<T>::value>>
bool operator==(T lhs, const BigInt& rhs)
{
    return rhs == lhs;
}

template <typename T, typename = std::enable_if_t<true == std::is_integral<T>::value>>
bool operator!=(T lhs, const BigInt& rhs)
{
    return rhs != lhs;
}

template <typename T, typename = std::enable_if_t<true == std::is_integral<T>::value>>
bool operator>(T lhs, const BigInt& rhs)
{
    return rhs < lhs;
}

template <typename T, typename = std::enable_if_t<true == std::is_integral<T>::value>>
bool operator>=(T lhs, const BigInt& rhs)
{
    return rhs <= lhs;
}

template <typename T, typename = std::enable_if_t<true == std::is_integral<T>::value>>
bool operator<(T lhs, const BigInt& rhs)
{
    return rhs > lhs;
}

template <typename T, typename = std::enable_if_t<true == std::is_integral<T>::value>>
bool operator<=(T lhs, const BigInt& rhs)
{
    return rhs >= lhs;
}

#endif // BIGINT_H
