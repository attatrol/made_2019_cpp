#include <iostream>
#include <vector>

#include "vector.h"

template <typename T>
bool testIterators(Vector<T> v)
{
    if (v.end() - v.begin() != v.size() && v.rbegin() - v.rend() != v.size())
    {
        return false;
    }
    std::size_t i = 0;
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        if (*it != v[i++])
        {
            return false;
        }
    }
    i = v.size() - 1;
    for (auto it = v.rbegin(); it != v.rend(); ++it)
    {
        if (*it != v[i--])
        {
            return false;
        }
    }
    return true;
}

template <typename T>
bool equal(Vector<T> v1, std::vector<T> v2)
{
    if (!testIterators(v1))
    {
        return false;
    }
    if (v1.size() != v2.size())
    {
        return false;
    }
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        if (v1[i] != v2[i])
        {
            return false;
        }
    }
    return true;
}

bool testCtor()
{
    Vector<int> v1;
    Vector<int> v2(10);
    Vector<int> v3(5, 42);
    std::vector<int> v3_e(5, 42);
    Vector<int> v4{1, 2, 3, 4, 5};
    std::vector<int> v4_e{1, 2, 3, 4, 5};
    return !v1.size() && v2.size() == 10 && equal(v3, v3_e) && equal(v4, v4_e);
}

bool testFill()
{
    Vector<int> v(42, 42);
    std::vector<int> v_e(42, 42);
    v.resize(100, -42);
    v_e.resize(100, -42);
    v.resize(50, 0);
    v_e.resize(50, 0);
    return equal(v, v_e);
}

struct A
{
    int a_;
    A(): a_(0)
    {
    }
    A(int a): a_(a)
    {
    }
    bool operator==(const A& other)
    {
        return a_ == other.a_;
    }
    bool operator!=(const A& other)
    {
        return a_ != other.a_;
    }
};

bool testPushPop()
{
    Vector<A> v;
    std::vector<A> v_e;
    v.push_back(1);
    v_e.push_back(1);
    v.emplace_back(-1);
    v_e.emplace_back(-1);
    if (!equal(v, v_e))
    {
        return false;
    }
    v.clear();
    v_e.clear();
    if (!equal(v, v_e))
    {
        return false;
    }
    v.push_back(2);
    v_e.push_back(2);
    v.push_back(3);
    v_e.push_back(3);
    v.push_back(4);
    v_e.push_back(4);
    v.pop_back();
    v_e.pop_back();
    v.push_back(1);
    v_e.push_back(1);
    return equal(v, v_e);
}

bool testCopy()
{
    Vector<A> v{1, 2, 3, 4, 5};
    std::vector<A> v_e{1, 2, 3, 4, 5};
    Vector<A> v_copy1(v);
    std::vector<A> v_copy1_e(v_e);
    Vector<A> v_copy2(Vector<A>{1, 2, 3, 4, 5});
    std::vector<A> v_copy2_e(std::vector<A>{1, 2, 3, 4, 5});
    Vector<A> v_copy3 = v;
    std::vector<A> v_copy3_e = v_e;
    Vector<A> v_copy4 = Vector<A>{1, 2, 3, 4, 5};
    std::vector<A> v_copy4_e = std::vector<A>{1, 2, 3, 4, 5};
    return equal(v, v_e) && equal(v_copy1, v_copy1_e) && equal(v_copy2, v_copy2_e) && equal(v_copy3, v_copy3_e) && equal(v_copy4, v_copy4_e);
}

int main()
{
    std::cout << "Test run started" << std::endl;

    if (!testCtor())
    {
        std::cout << "Failed ctor test" << std::endl;
    }
    if (!testFill())
    {
        std::cout << "Failed filling with default values test" << std::endl;
    }
    if (!testPushPop())
    {
        std::cout << "Failed push_back/pop_back/emplace_back test" << std::endl;
    }
    if (!testCopy())
    {
        std::cout << "Failed copy test" << std::endl;
    }

    std::cout << "Test run ended" << std::endl;

    return 0;
}
