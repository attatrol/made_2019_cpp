#ifndef ITERATOR_H
#define ITERATOR_H

#include <cstdlib>
#include <iterator>

template <class T>
class Iterator
{
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;
private:
    pointer p_;
public:
    explicit Iterator(pointer p): p_(p)
    {
    }
    Iterator(const Iterator& it) : p_(it.p_)
    {
    }
    pointer operator->() const
    {
        return p_;
    }
    reference operator*() const
    {
        return *p_;
    }
    reference operator[](difference_type i)
    {
        return p_[i];
    }
    bool operator==(const Iterator<T>& other) const
    {
        return p_ == other.p_;
    }
    bool operator!=(const Iterator<T>& other) const
    {
        return p_ != other.p_;
    }
    bool operator<(const Iterator<T>& other) const
    {
        return p_ < other.p_;
    }
    bool operator>(const Iterator<T>& other) const
    {
        return p_ > other.p_;
    }
    bool operator<=(const Iterator<T>& other) const
    {
        return p_ <= other.p_;
    }
    bool operator>=(const Iterator<T>& other) const
    {
        return p_ >= other.p_;
    }
    Iterator& operator++()
    {
        ++p_;
        return *this;
    }
    Iterator operator++(int)
    {
        return Iterator(p_++);
    }
    Iterator& operator--()
    {
        --p_;
        return *this;
    }
    Iterator operator--(int)
    {
        return Iterator(p_--);
    }
    Iterator operator+(difference_type offset) const
    {
        return Iterator(p_ + offset);
    }
    Iterator operator-(difference_type offset) const
    {
        return Iterator(p_ - offset);
    }
    difference_type operator-(const Iterator& it) const
    {
        return p_ - it.p_;
    }
};

#endif // ITERATOR_H
