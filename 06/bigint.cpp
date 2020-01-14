#include <algorithm>
#include <assert.h>

#include "bigint.h"

BigInt::BigInt(): m_cells(nullptr), m_capacity(0), m_size(0), m_isPositive(true)
{
    // std::cout << "copyEx\n";
}
BigInt::BigInt(cell_t* cells, std::size_t capacity, std::size_t size, bool isPositive): m_cells(cells), m_capacity(capacity), m_size(size), m_isPositive(isPositive)
{
}
BigInt::BigInt(const BigInt& other, std::size_t capacity): m_cells(new cell_t[capacity]), m_capacity(capacity), m_size(other.m_size), m_isPositive(other.m_isPositive)
{
    assert(other.m_size <= capacity);
    std::copy(other.m_cells, other.m_cells + other.m_size, m_cells);
}
BigInt::BigInt(const BigInt& other): BigInt(other, other.m_capacity)
{
    // std::cout << "copy\n";
}
BigInt& BigInt::operator=(const BigInt& other)
{
    // std::cout << "move\n";
    if (this != &other) {
        if (other.m_size > m_capacity) {
            cell_t* cells = new cell_t[other.m_size];
            delete[] m_cells;
            m_cells = cells;
            m_capacity = other.m_size;
        }
        m_size = other.m_size;
        m_isPositive = other.m_isPositive;
        std::copy(other.m_cells, other.m_cells + other.m_size, m_cells);
    }
    return *this;
}
BigInt::BigInt(BigInt&& other) noexcept: m_cells(other.m_cells), m_capacity(other.m_capacity), m_size(other.m_size), m_isPositive(other.m_isPositive)
{
    other.m_cells = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
    other.m_isPositive = true;
}
BigInt& BigInt::operator=(BigInt&& other) noexcept
{
    if (this != &other) {
        delete[] m_cells;
        m_cells = other.m_cells;
        other.m_cells = nullptr;
        m_capacity = other.m_capacity;
        other.m_capacity = 0;
        m_size = other.m_size;
        other.m_size = 0;
        m_isPositive = other.m_isPositive;
        other.m_isPositive = true;
    }
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

int BigInt::compareCells(const cell_t* cells, const std::size_t size) const
{
    assert(!size || cells[size - 1] > 0);
    if (m_size == size)
    {
        for (std::size_t i = m_size - 1; i < m_size; --i)
        {
            if (m_cells[i] > cells[i])
            {
                return 1;
            }
            else if (m_cells[i] < cells[i])
            {
                return -1;
            }
        }
        return 0;
    }
    else if (m_size > size)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}
int BigInt::compare(const BigInt& other) const
{
    if (!m_size)
    {
        return !other.m_size ? 0 : (other.m_isPositive ? -1 : 1);
    }
    else if (m_isPositive)
    {
        return other.m_isPositive ? compareCells(other.m_cells, other.m_size) : 1;
    }
    else
    {
        return other.m_isPositive ? -1 : -compareCells(other.m_cells, other.m_size);
    }
}
template<typename T> int BigInt::compareInteger(T value)
{
    // static_assert(std::is_integral<T>::value, "Integer argument required");
    static const std::size_t SIZE = CELL_SIZE(T);

    if (m_isPositive)
    {
        if (value < 0 || m_size > SIZE)
        {
            return 1;
        }
        if (!value)
        {
            return m_size == 0 ? 0 : 1;
        }
        for (std::size_t i = m_size - 1; i < m_size; --i)
        {
            T cell = (value >> (i * CELL_BIT_COUNT)) & CELL_MASK;
            if (m_cells[i] > cell)
            {
                return 1;
            }
            else if (m_cells[i] < cell)
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        if (value >= 0)
        {
            return -1;
        }
        for (std::size_t i = m_size - 1; i < m_size; --i)
        {
            T cell = (value >> (i * CELL_BIT_COUNT)) & CELL_MASK;
            if (m_cells[i] > cell)
            {
                return -1;
            }
            else if (m_cells[i] < cell)
            {
                return 1;
            }
        }
        return 0;
    }
}
void BigInt::expandCapacity(std::size_t capacity)
{
    assert(capacity > m_size);
    assert(m_size <= m_capacity);
    cell_t* cells = new cell_t[capacity];
    if (m_cells)
    {
        std::copy(m_cells, m_cells + m_size, cells);
        delete[] m_cells;
    }
    m_cells = cells;
    m_capacity = capacity;
}
void BigInt::carry(cell_t carryValue)
{
    if (m_size == m_capacity)
    {
        expandCapacity(m_capacity + 1);
    }
    m_cells[m_size++] = carryValue;
}
void BigInt::shrinkSize()
{
    while (m_size > 0 && !m_cells[m_size - 1])
    {
        --m_size;
    }
}
void BigInt::addCells(const cell_t* cells, const std::size_t size)
{
    assert(size <= m_capacity);
    cell_t overflow = 0;
     if (m_size >= size)
     {
        for (std::size_t i = 0; i < size; ++i)
        {
            m_cells[i] += cells[i] + overflow;
            overflow = m_cells[i] >> CELL_BIT_COUNT;
            assert(overflow == 0 || overflow == 1);
            m_cells[i] &= CELL_MASK;
        }
        for (std::size_t i = size; i < m_size; ++i)
        {
            m_cells[i] += overflow;
            overflow = m_cells[i] >> CELL_BIT_COUNT;
            assert(overflow == 0 || overflow == 1);
            if (!overflow)
            {
                break;
            }
            m_cells[i] &= CELL_MASK;
        }
     }
     else
     {
         for (std::size_t i = 0; i < m_size; ++i)
         {
             m_cells[i] += cells[i] + overflow;
             overflow = m_cells[i] >> CELL_BIT_COUNT;
             assert(overflow == 0 || overflow == 1);
             m_cells[i] &= CELL_MASK;
         }
         for (std::size_t i = m_size; i < size; ++i)
         {
             m_cells[i] = cells[i] + overflow;
             overflow = m_cells[i] >> CELL_BIT_COUNT;
             assert(overflow == 0 || overflow == 1);
             m_cells[i] &= CELL_MASK;
         }
         m_size = size;
     }
     if (overflow)
     {
         carry(1);
     }
}
void BigInt::addSelf()
{
    cell_t overflow = 0;
    for (std::size_t i = 0; i < m_size; ++i)
    {
        m_cells[i] = (m_cells[i] << 1) + overflow;
        overflow = m_cells[i] >> CELL_BIT_COUNT;
        assert(overflow == 0 || overflow == 1);
        m_cells[i] &= CELL_MASK;
    }
    if (overflow)
    {
        carry(1);
    }
}
void BigInt::addCellsCopy(const cell_t *cells, const std::size_t size)
{
    if (size > m_capacity)
    {
        expandCapacity(size);
    }
    addCells(cells, size);
}
void BigInt::addCellsSwap(BigInt&& value)
{
    if (value.m_size > m_capacity)
    {
        std::swap(m_cells, value.m_cells);
        std::swap(m_size, value.m_size);
        std::swap(m_capacity, value.m_capacity);
        // m_isPositive stays as is
    }
    addCells(value.m_cells, value.m_size);
}
void BigInt::incCells()
{
    cell_t overflow = 1;
    for (std::size_t i = 0; i < m_size; ++i)
    {
        m_cells[i] += overflow;
        overflow = m_cells[i] >> CELL_BIT_COUNT;
        assert(overflow == 0 || overflow == 1);
        if (!overflow)
        {
            break;
        }
        m_cells[i] &= CELL_MASK;
    }
    if (overflow)
    {
        carry(1);
    }
}
void BigInt::subCells(const cell_t *cells, const std::size_t size)
{
    assert(compareCells(cells, size) != -1);
    cell_t overflow = 1; // дополнительный код
    for (std::size_t i = 0; i < size; ++i)
    {
        cell_t twosComplement = ~cells[i] & CELL_MASK;
        m_cells[i] += twosComplement + overflow;
        overflow = (m_cells[i] >> CELL_BIT_COUNT) & 1;
        assert(overflow == 0 || overflow == 1);
        m_cells[i] &= CELL_MASK;
    }
    for (std::size_t i = size; i < m_size; ++i)
    {
        if (!overflow)
        {
            if (!m_cells[i])
            {
                assert(i < m_size - 1); // assert(compareCells(cells, size) == 1);
                m_cells[i] = CELL_MASK;
            }
            else
            {
                --m_cells[i];
                break;
            }
        }
        else
        {
            break;
        }
    }
    shrinkSize();
    assert(m_cells[m_size - 1] > 0);
}
void BigInt::subCellsCopy(const cell_t *cells, const std::size_t size)
{
    if (!size)
    {
        return;
    }
    const int cmpResult = compareCells(cells, size);
    if (cmpResult == 0)
    {
        setZero();
    }
    else if (cmpResult == -1)
    {
        cell_t* cellsCopy = new cell_t[size];
        std::copy(cells, cells + size, cellsCopy);
        std::swap(m_cells, cellsCopy);
        const std::size_t sizeCopy = m_size;
        m_size = size;
        m_capacity = size;
        m_isPositive = !m_isPositive;
        subCells(cellsCopy, sizeCopy);
        delete[] cellsCopy;
    }
    else
    {
        subCells(cells, size);
    }
}
void BigInt::subCellsSwap(BigInt&& value)
{
    if (!value.m_size)
    {
        return;
    }
    const int cmpResult = compareCells(value.m_cells, value.m_size);
    if (cmpResult == 0)
    {
        setZero();
        return;
    }
    if (cmpResult == -1)
    {
        std::swap(m_cells, value.m_cells);
        std::swap(m_size, value.m_size);
        std::swap(m_capacity, value.m_capacity);
        m_isPositive = !m_isPositive;
    }
    subCells(value.m_cells, value.m_size);
}
void BigInt::decCells()
{
    assert(!isZero());
    for (std::size_t i = 0; i < m_size; ++i)
    {
        if (m_cells[i])
        {
            --m_cells[i];
            break;
        }
        else
        {
            m_cells[i] = CELL_MASK;
        }
    }
    shrinkSize();
}
void BigInt::mulCellAdd(const cell_t value, const std::size_t pos, cell_t* product, const std::size_t size)
{
    if (!value)
    {
        return;
    }
    assert(value <= CELL_MASK);
    const cell_t firstCellValue = product[pos] + value;
    product[pos] = firstCellValue & CELL_MASK;
    cell_t overflow = firstCellValue >> CELL_BIT_COUNT;
    for (std::size_t i = pos + 1; i < size; ++i)
    {
        assert(overflow == 0 || overflow == 1);
        if (!overflow)
        {
            return;
        }
        product[i] += overflow;
        overflow = product[i] >> CELL_BIT_COUNT;
        product[i] &= CELL_MASK;
    }
    assert(!overflow); // product overflow shall never happen
}
void BigInt::mulCells(const cell_t* factor1, std::size_t size1, const cell_t* factor2, std::size_t size2, cell_t* product, const std::size_t size)
{
    assert(size1 && size2 && size1 + size2 <= size);
    std::fill(product, product + size, 0);

    for (std::size_t productIdx = 0; productIdx < size; ++productIdx)
    {
        cell_t acc0 = 0, acc1 = 0, acc2 = 0;
        std::size_t counter = 0;
        for(std::size_t i1 = size2 > productIdx ? 0 : productIdx - size2 + 1 , i2 = productIdx - i1; i1 < size1 && i2 <= productIdx; ++i1, --i2)
        {
            assert(i1 < size1 && i2 < size2);
            const cell_t cellProduct = factor1[i1] * factor2[i2];
            acc0 += (cellProduct & CELL_MASK);
            acc1 += (cellProduct >> CELL_BIT_COUNT);
            if (acc0 > CELL_MASK)
            {
                ++acc1;
                acc0 -= (CELL_MASK + 1);
                assert(acc0 <= CELL_MASK);
            }
            if (acc1 > CELL_MASK)
            {
                ++acc2;
                acc1 -= (CELL_MASK + 1);
                assert(acc1 <= CELL_MASK);
            }
            counter++;
            // this branch will never be executed: there is simply not enough memory to allocate numbers  of size CELL_MASK *  sizeof(cell_t) bytes
            // yet it is added for the sake of completeness
            if (__builtin_expect(counter == CELL_MASK, false))
            {
                mulCellAdd(acc0, productIdx, product, size);
                mulCellAdd(acc1, productIdx + 1, product, size);
                mulCellAdd(acc2, productIdx + 2, product, size);
                acc0 = acc1 = acc2 = 0;
                counter = 0;
            }
        }
        mulCellAdd(acc0, productIdx, product, size);
        mulCellAdd(acc1, productIdx + 1, product, size);
        mulCellAdd(acc2, productIdx + 2, product, size);
    }
}

////////////////////////////////////////////////////////////////////////////////

void BigInt::add(const BigInt& value)
{
    if (&value == this)
    {
        addSelf();
    }
    else if (m_isPositive != value.m_isPositive)
    {
        subCellsCopy(value.m_cells, value.m_size);
    }
    else
    {
        addCellsCopy(value.m_cells, value.m_size);
    }
}
void BigInt::add(BigInt&& value)
{
    if (&value == this)
    {
        addSelf();
    }
    if (m_isPositive != value.m_isPositive)
    {
        subCellsSwap(std::move(value));
    }
    else
    {
        addCellsSwap(std::move(value));
    }

}
void BigInt::sub(const BigInt& value)
{
    if (&value == this)
    {
        setZero();
    }
    else if (m_isPositive != value.m_isPositive)
    {
        addCellsCopy(value.m_cells, value.m_size);
    }
    else
    {
        subCellsCopy(value.m_cells, value.m_size);
    }
}
void BigInt::sub(BigInt&& value)
{
    if (&value == this)
    {
        setZero();
    }
    else if (m_isPositive != value.m_isPositive)
    {
        addCellsSwap(std::move(value));
    }
    else
    {
        subCellsSwap(std::move(value));
    }
}
void BigInt::mul(const BigInt& value)
{
    if (!m_size)
    {
        return;
    }
    if (!value.m_size)
    {
        setZero();
        return;
    }
    const std::size_t productSize = m_size + value.m_size;
    if (productSize <= m_capacity)
    {
        cell_t* factor1 = new cell_t[m_size];
        std::copy(m_cells, m_cells + m_size, factor1);
        mulCells(factor1, m_size, value.m_cells, value.m_size, m_cells, productSize);
        delete[] factor1;
    }
    else
    {
        cell_t* product = new cell_t[productSize];
        mulCells(m_cells, m_size, value.m_cells, value.m_size, product, productSize);
        delete[] m_cells;
        m_cells = product;
        m_capacity = productSize;
    }
    m_size = productSize;
    shrinkSize();
    m_isPositive = m_isPositive == value.m_isPositive;
}
void BigInt::mul(BigInt &&value)
{
    if (!m_size)
    {
        return;
    }
    if (!value.m_size)
    {
        setZero();
        return;
    }
    const std::size_t productSize = m_size + value.m_size;
    if (productSize <= value.m_capacity)
    {
        cell_t* factor2 = new cell_t[value.m_size];
        std::copy(value.m_cells, value.m_cells + value.m_size, factor2);
        mulCells(m_cells, m_size, factor2, value.m_size, value.m_cells, productSize);
        delete[] m_cells;
        m_cells = value.m_cells;
        m_capacity = value.m_capacity;
        value.m_capacity = value.m_size;
        value.m_cells = factor2;
    }
    else if (productSize <= m_capacity)
    {
        cell_t* factor1 = new cell_t[m_size];
        std::copy(m_cells, m_cells + m_size, factor1);
        mulCells(factor1, m_size, value.m_cells, value.m_size, m_cells, productSize);
        delete[] factor1;
    }
    else
    {
        cell_t* product = new cell_t[productSize];
        mulCells(m_cells, m_size, value.m_cells, value.m_size, product, productSize);
        delete[] m_cells;
        m_cells = product;
        m_capacity = productSize;
    }
    m_size = productSize;
    shrinkSize();
    m_isPositive = m_isPositive == value.m_isPositive;
}

////////////////////////////////////////////////////////////////////////////////

BigInt& BigInt::operator++()
{
    if (isZero())
    {
        m_isPositive = true;
        incCells();
    }
    else if (m_isPositive)
    {
        incCells();
    }
    else
    {
        decCells();
    }
    return *this;
}
BigInt& BigInt::operator--()
{
    if (isZero())
    {
        carry(1);
        m_isPositive = false;
    }
    else if (m_isPositive)
    {
        decCells();
    }
    else
    {
        incCells();
    }
    return *this;
}

BigInt BigInt::operator++(int)
{
    BigInt result(*this);
    ++*this;
    return result;
}
BigInt BigInt::operator--(int)
{
    BigInt result(*this);
    --*this;
    return result;
}
BigInt& BigInt::operator+=(const BigInt& rhs)
{
    add(rhs);
    return *this;
}
BigInt& BigInt::operator+=(BigInt&& rhs)
{
    add(rhs);
    return *this;
}
BigInt& BigInt::operator-=(const BigInt& rhs)
{
    sub(rhs);
    return *this;
}
BigInt& BigInt::operator-=(BigInt&& rhs)
{
    sub(std::move(rhs));
    return *this;
}
BigInt& BigInt::operator*=(const BigInt& rhs)
{
    mul(rhs);
    return *this;
}
BigInt& BigInt::operator*=(BigInt&& rhs)
{
    mul(std::move(rhs));
    return *this;
}
BigInt BigInt::operator+(const BigInt& rhs) const &
{
    BigInt result(*this, rhs.m_size > m_size ? rhs.m_size : m_size);
    result.add(rhs);
    return result;
}
BigInt BigInt::operator+(const BigInt& rhs) &&
{
    add(rhs);
    return std::move(*this);
}
BigInt BigInt::operator+(BigInt&& rhs) const &
{
    rhs.add(*this);
    return std::move(rhs);
}
BigInt BigInt::operator+(BigInt&& rhs) &&
{
    add(std::move(rhs));
    return std::move(*this);
}
BigInt BigInt::operator-(const BigInt& rhs) const &
{
    BigInt result(*this, rhs.m_size > m_size ? rhs.m_size : m_size);
    result.sub(rhs);
    return result;
}
BigInt BigInt::operator-(const BigInt& rhs) &&
{
    sub(rhs);
    return std::move(*this);
}
BigInt BigInt::operator-(BigInt&& rhs) const &
{
    rhs.sub(*this);
    rhs.m_isPositive = !rhs.m_isPositive;
    return std::move(rhs);
}
BigInt BigInt::operator-(BigInt&& rhs) &&
{
    sub(std::move(rhs));
    return std::move(*this);
}
BigInt BigInt::operator*(const BigInt& rhs) const &
{
    if (!m_size || !rhs.m_size)
    {
        return BigInt();
    }
    const std::size_t size = m_size + rhs.m_size;
    cell_t* product = new cell_t[size];
    mulCells(m_cells, m_size, rhs.m_cells, rhs.m_size, product, size);
    BigInt result = BigInt(product, size, size, m_isPositive == rhs.m_isPositive);
    result.shrinkSize();
    return result;
}
BigInt BigInt::operator*(const BigInt& rhs) &&
{
    mul(rhs);
    return std::move(*this);
}
BigInt BigInt::operator*(BigInt&& rhs) const &
{
    rhs.mul(*this);
    return std::move(rhs);
}
BigInt BigInt::operator*(BigInt&& rhs) &&
{
    mul(std::move(rhs));
    return std::move(*this);
}

////////////////////////////////////////////////////////////////////////////////

BigInt::cell_t BigInt::divModShort(cell_t* divident, std::size_t& size, const std::size_t divisor)
{
    assert(divisor > 0 && divisor <= CELL_MASK);
    cell_t mod = 0;
    for(std::size_t i = size - 1; i < size; --i)
    {
        cell_t cellValue = divident[i] + (mod << CELL_BIT_COUNT);
        assert(cellValue || i < size - 1);
        divident[i] = cellValue / divisor;
        mod = cellValue % divisor;
        assert(divident[i] <= CELL_MASK);
        assert(mod <= CELL_MASK);
    }
    while(size && !divident[size - 1])
    {
        --size;
    }
    return mod;
}

char* BigInt::toDecimal() const
{
    char* result = new char[sizeof(cell_t) * m_size * 100 + 2]; // TODO : log10
    if (isZero())
    {
        result[0] = '0';
        result[1] = '\0';
        return result;
    }
    std::size_t resultIdx = 0;
    cell_t* divident = new cell_t[m_size];
    std::copy(m_cells, m_cells + m_size, divident);
    std::size_t dividentIdx = m_size;
    while(true)
    {
        cell_t mod = divModShort(divident, dividentIdx, TO_DECIMAL_CELL_DIVISOR);
        if (!dividentIdx)
        {
            while(mod)
            {
                cell_t div2 = mod / 10;
                char mod2 = mod % 10;
                mod = div2;
                result[resultIdx++] = mod2 + '0';
            }
            break;
        }
        for (std::size_t i = 0; i < DECIMAL_CELL_DIGITS; ++i)
        {
            cell_t div2 = mod / 10;
            char mod2 = mod % 10 + '0';
            mod = div2;
            result[resultIdx++] = mod2;
        }
    }
    delete[] divident;
    if (!m_isPositive)
    {
        result[resultIdx++] = '-';
    }
    for (std::size_t i = 0; i < resultIdx / 2; ++i)
    {
        std::swap(result[i], result[resultIdx - i - 1]);
    }
    result[resultIdx++] = '\0';
    return result;
}
