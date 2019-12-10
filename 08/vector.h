#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>
#include <type_traits>

#include "allocator.h"
#include "iterator.h"

template <typename T, typename Alloc = Allocator<T>>
class Vector
{
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using allocator_type = Alloc;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = typename std::allocator_traits<Alloc>::pointer;
    using iterator = Iterator<T>;
    using const_iterator = const iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
    Alloc alloc_;
    size_type size_;
    size_type capacity_;
    value_type* data_;
public:
    explicit Vector(size_type count);
    Vector(): Vector(0)
    {
    }
    Vector(size_type count, const value_type& defaultValue);
    Vector(std::initializer_list<value_type> list);
    Vector(const Vector<T, Alloc>& other);
    Vector(Vector<T, Alloc>&& other);
    Vector<T, Alloc>& operator=(const Vector<T, Alloc>& other);
    Vector<T, Alloc>& operator=(Vector<T, Alloc>&& other);
    ~Vector();

    iterator begin() noexcept;
    iterator end() noexcept;
    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_iterator cbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crend() const noexcept;
    size_type size() const noexcept
    {
        return size_;
    }
    size_type capacity() const noexcept
    {
        return capacity_;
    }
    bool empty() const noexcept
    {
        return size_;
    }
    const_reference operator[](size_type index) const
    {
        return data_[index];
    }
    reference operator[](size_type index)
    {
        return data_[index];
    }
    const_reference at(size_type index) const;
    reference at(size_type index);

    void reserve(size_type capacity);
    void resize(size_type newSize);
    void resize(size_type newSize, const value_type& defaultValue);

    void push_back(value_type&& value);
    void pop_back();
    template<class... VT>
    void emplace_back(VT&&... values);
    void clear() noexcept;
};

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>::size_type count):
    alloc_(Alloc()), size_(count), capacity_(count < 8 ? 8 : count), data_(alloc_.allocate(capacity_))
{
    for (size_type i = 0; i < size_; ++i)
    {
        std::allocator_traits<Alloc>::construct(alloc_, data_+i);
    }
}
template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_type count, const value_type& defaultValue):
    alloc_(Alloc()), size_(count), capacity_(count), data_(alloc_.allocate(capacity_))
{
    for (size_type i = 0; i < size_; ++i)
    {
        std::allocator_traits<Alloc>::construct(alloc_, data_+i, defaultValue);
    }
}
template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<value_type> list):
    alloc_(Alloc()), size_(list.size()), capacity_(size_), data_(alloc_.allocate(size_))
{
    for (size_type i = 0; i < size_; ++i)
    {
        std::allocator_traits<Alloc>::construct(alloc_, data_+i, *(list.begin()+i));
    }
}
template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>& other): Vector(other.size_)
{
    std::copy(other.cbegin(), other.cend(), data_);
}
template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>&& other): Vector()
{
    std::swap(alloc_, other.alloc_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(data_, other.data_);
}
template<typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector<T, Alloc>& other)
{
    if(&other == this)
    {
        return *this;
    }
    clear();
    if(capacity_ < other.size_)
    {
        reserve(other.size_);
    }
    std::copy(other.begin(), other.end(), data_);
    size_ = other.size_;
    return *this;
}
template<typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(Vector<T, Alloc>&& other)
{
    if(&other == this)
    {
        return *this;
    }
    std::swap(alloc_, other.alloc_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(data_, other.data_);
    return *this;
}
template<typename T, typename Alloc>
Vector<T, Alloc>::~Vector()
{
    clear();
    alloc_.deallocate(data_, capacity_);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::begin() noexcept
{
    return iterator(data_);
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::end() noexcept
{
    return iterator(data_ + size_);
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rbegin() noexcept
{
    return reverse_iterator(end());
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rend() noexcept
{
    return reverse_iterator(begin());
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cbegin() const noexcept
{
    return const_iterator(data_);
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cend() const noexcept
{
    return const_iterator(data_ + size_);
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crbegin() const noexcept
{
    return const_reverse_iterator(cend());
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crend() const noexcept
{
    return const_reverse_iterator(cbegin());
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference Vector<T, Alloc>::at(size_type index) const
{
    if (index >= size_)
    {
        throw std::range_error("Index " +std::to_string(index) + " is not bounded by vector size " + std::to_string(size_));
    }
    return *this[index];
}
template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference Vector<T, Alloc>::at(size_type index)
{
    if (index >= size_)
    {
        throw std::range_error("Index " +std::to_string(index) + " is not bounded by vector size " + std::to_string(size_));
    }
    return *this[index];
}
template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(value_type&& value)
{
    if(size_ == capacity_)
    {
        reserve(2 * capacity_);
    }
    std::allocator_traits<Alloc>::construct(alloc_, data_ + size_++, std::forward<value_type>(value));
}
template<typename T, typename Alloc>
void Vector<T, Alloc>::pop_back()
{
    std::allocator_traits<Alloc>::destroy(alloc_, data_ + --size_);
}
template<typename T, typename Alloc> template<class... VT>
void Vector<T, Alloc>::emplace_back(VT&&... values)
{
    if(size_ == capacity_)
    {
        reserve(2 * capacity_);
    }
    std::allocator_traits<Alloc>::construct(alloc_, data_ + size_++, std::move(values)...);
}
template<typename T, typename Alloc>
void Vector<T, Alloc>::reserve(size_type capacity)
{
    if (capacity <= capacity_)
    {
        return;
    }
    pointer newData(alloc_.allocate(capacity));
    for (size_type i = 0; i < size_; ++i)
    {
        std::allocator_traits<Alloc>::construct(alloc_, newData + i, std::move(*(data_ + i)));
        std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
    }
    alloc_.deallocate(data_, capacity_);
    capacity_ = capacity;
    data_ = newData;
}
template<typename T, typename Alloc>
void Vector<T, Alloc>::resize(size_type newSize)
{
    reserve(newSize);
    for (size_type i = newSize; i < size_; ++i)
    {
        std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
    }
    for (size_type i = size_; i < newSize; ++i)
    {
        std::allocator_traits<Alloc>::construct(alloc_, data_ + i);
    }
    size_ = newSize;
}
template<typename T, typename Alloc>
void Vector<T, Alloc>::resize(size_type newSize, const value_type& defaultValue)
{
    reserve(newSize);
    for (size_type i = newSize; i < size_; ++i)
    {
        std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
    }
    for (size_type i = size_; i < newSize; ++i)
    {
        std::allocator_traits<Alloc>::construct(alloc_, data_ + i, defaultValue);
    }
    size_ = newSize;
}
template<typename T, typename Alloc>
void Vector<T, Alloc>::clear() noexcept
{
    for (size_type i = 0; i < size_; ++i)
    {
        std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
    }
    size_ = 0;
}


#endif // VECTOR_H
