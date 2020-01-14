#include <iostream>
#include <limits.h>
#include <sstream>

#include "bigint.h"

const long MAX_LONG = std::numeric_limits<long>::max();
const long MIN_LONG = std::numeric_limits<long>::min();

std::pair<const long, const long> COMPARISON_PAIRS[] = {
    { 0, 0 },
    { -1, -1 },
    { 42, 42 },
    { 1, 0 },
    { -1, 0 },
    { -80, 80 },
    { MAX_LONG, MIN_LONG + 1 },
    { -1, 200 },
    { 10000000, 10000001 },
    { MAX_LONG, MAX_LONG },
    { MIN_LONG, MIN_LONG },
    { MIN_LONG / 10, MIN_LONG / 10 },
    { MIN_LONG, MAX_LONG },
    { MAX_LONG, MIN_LONG },
    { 0, MAX_LONG },
    { 0, MIN_LONG },
    { -1, MIN_LONG },
    { -1, MAX_LONG }
};

bool testComparison(const long a, const long b)
{
    BigInt a_l = a, 
           b_l = b, 
           a_l1 = a_l * MAX_LONG,
           b_l1 = b_l * MAX_LONG,
           a_l2 = a_l * (MIN_LONG + 42),
           b_l2 = b_l * (MIN_LONG + 42);
           
    if (a_l != a || b_l != b || a != a_l || b != b_l)
    {
        return false;
    }
    if (a == b)
    {
        return a_l == b_l && a_l1 == b_l1 && a_l2 == b_l2;
    }
    else if (a > b)
    {
        return a_l > b_l && a_l1 > b_l1 && a_l2 < b_l2;
    }
    else
    {
        return a_l < b_l && a_l1 < b_l1 && a_l2 > b_l2;
    }
}

struct ArithmeticProgression {
    const long a0;
    const long d;
    const std::size_t n;
};

ArithmeticProgression PROGRESSIONS[] = {
    { 0, 1, 100 },
    { 0, 10000, 10000 },
    { 1, 1000, 1000 },
    { MAX_LONG, 0, 10 },
    { MAX_LONG, -25, 1000 },
    { MAX_LONG - 1, 0, 10 },
    { MIN_LONG, 0, 1000 },
    { MAX_LONG, 0, 10000 },
    
    { MAX_LONG, -MAX_LONG / 100000,  10000 },
    { 0,        -MAX_LONG / 10000,   1000  },
    { -3,       -MAX_LONG / 100000,  99999 },
    { 11,       -MAX_LONG / 1000000, 1000000 },
    { -21,      -MAX_LONG / 1000000, 999999 },
    { 0,        -MAX_LONG / 10000,   10000 },
    { -3,       -MAX_LONG / 100000,  99999 },
    { 11,       -MAX_LONG / 1000000, 1000000 },
    { -21,      -MAX_LONG / 1000000, 999999 },

    { MIN_LONG, MAX_LONG / 100000,  10000 },
    { 0,        MAX_LONG / 10000,   1000  },
    { -3,       MAX_LONG / 100000,  99999 },
    { 11,       MAX_LONG / 1000000, 1000000 },
    { -21,      MAX_LONG / 1000000, 999999 },
    { 0,        MAX_LONG / 10000,   10000 },
    { -3,       MAX_LONG / 100000,  99999 },
    { 11,       MAX_LONG / 1000000, 1000000 },
    { -21,      MAX_LONG / 1000000, 999999 },
};

bool testArithmeticProgression(const long a0, const long d, const std::size_t n, bool verbose = false)
{
    if (verbose)
    {
        std::cout << "a0 = " << a0 << " d = " << d << " n = " << n << std::endl;
    }
    BigInt a0_l = a0, d_l = d, n_l = n;
    if (verbose)
    {
        std::cout << "a0_l = " << a0_l << " d_l = " << d_l << " n_l = " << n_l << std::endl;
    }
    BigInt sum = n_l * (a0_l + a0 + (n_l - 1) * d);
    if (verbose)
    {
        std::cout << "sum = " << sum << std::endl;
    }

    BigInt acc = a0;
    BigInt accNeg = sum - 2 * a0_l;
    BigInt counter = 0;
    if (verbose)
    {
        std::cout << "counter = " << counter << " acc = " << acc << " accNeg = " << accNeg << std::endl;
    }

    int var = 0;
    
    const bool safeExpressionLimit = a0 == MIN_LONG || d == MIN_LONG ? 24 : 32;

    while(++counter < n)
    {
        BigInt value;
        switch (var)
        {
            // a lot of different ways to do the same thing to ensure
            // that all operators were called
            case 0:  value = a0 +   -d_l * -counter;   break;
            case 1:  value = a0 +   -counter * -d_l;   break;
            case 2:  value = a0 +   d_l * counter; break;
            case 3:  value = a0 +   counter * d_l; break;
            case 4:  value = a0_l + d * counter;   break;
            case 5:  value = a0_l + counter * d;   break;
            case 6:  value = a0_l + d_l * counter; break;
            case 7:  value = a0_l + counter * d_l; break;
            case 8:  value = -d_l * -counter +     a0; break;
            case 9:  value = -counter * -d_l +     a0; break;
            case 10: value = d_l * counter +   a0; break;
            case 11: value = counter * d_l +   a0; break;
            case 12: value = d * counter +   a0_l; break;
            case 13: value = counter * d +   a0_l; break;
            case 14: value = d_l * counter + a0_l; break;
            case 15: value = counter * d_l + a0_l; break;
            case 16: value = a0 +   -d_l * -counter; break;
            case 17: value = a0 +   -counter * -d_l; break;
            case 18: value = a0_l + -d_l * -counter; break;
            case 19: value = a0_l + -counter * -d_l; break;
            case 20: value = d * counter -   -a0_l; break;
            case 21: value = counter * d -   -a0_l; break;
            case 22: value = d_l * counter - -a0_l; break;
            case 23: value = counter * d_l - -a0_l; break;
            case 24: value = a0 +   -d * -counter;   break;
            case 25: value = a0 +   -counter * -d;   break;
            case 26: value = a0_l + -d * -counter;   break;
            case 27: value = a0_l + -counter * -d;   break;
            case 28: value = d * counter -     -a0; break;
            case 29: value = counter * d -     -a0; break;
            case 30: value = d_l * counter -   -a0; break;
            case 31: value = counter * d_l -   -a0; break;
        }
        var = ++var % safeExpressionLimit;
        acc += value;
        accNeg -= 2 * value;
        if (verbose)
        {
            std::cout << "counter = " << counter << " value = " << value << " acc = " << acc << " accNeg = " << accNeg << std::endl;
        }
    }
    acc *= 2;
    if (verbose) {
        std::cout << "Final" << std::endl << "sum = " << sum << " acc = " << acc << " accNeg = " << accNeg << std::endl;
    }

    return  0 == accNeg
            && sum == acc;
}

bool testIncrement(const BigInt& a0, const std::size_t n, bool verbose = false)
{
    BigInt value0 = a0, value1 = a0, value2 = a0, value3 = a0;
    for (std::size_t i = 0; i < n; ++i)
    {
        value0++;
        ++value1;
        value2--;
        --value3;
        if (verbose) {
            std::cout << "i = " << i << " value0 = " << value0 << " value1 = " << value1 << " value2 = " << value2 << " value3 = " << value3 << std::endl;
        }
    }
    BigInt a0_inc = a0 + n, a0_dec = a0 - n;
    if (value0 != a0_inc || value1 != a0_inc || value2 != a0_dec || value3 != a0_dec)
    {
        return false;
    }
    for (std::size_t i = 0; i < n; ++i)
    {
        --value0;
        value1--;
        value2++;
        ++value3;
        if (verbose) {
            std::cout << "i = " << i << " value0 = " << value0 << " value1 = " << value1 << " value2 = " << value2 << " value3 = " << value3 << std::endl;
        }
    }
    if (verbose) {
        std::cout <<"value0 = " << value0 << " value1 = " << value1 << " value2 = " << value2 << " value3 = " << value3 << std::endl;
    }
    return a0 == value0 && a0 == value1 && a0 == value2 && a0 == value3;
}

long TO_DECIMAL_TEST_VALUES[] = { 0, 1, -1, 200, 3245352, -176554, MAX_LONG, -MAX_LONG, MIN_LONG, -42 };

bool testToDecimalString(long value, std::string str)
{
    std::stringstream stream;
    BigInt value_l = value;
    stream << value_l;
    std::string output;
    stream >> output;
    // std::cout << str << std::endl;
    // std::cout << output << std::endl;
    if (str != output)
    {
        return false;
    }
    if (value != 0)
    {
        for (std::size_t i = 0; i < 10; ++i)
        {
            value_l *= 1000;
            str += "000";
            stream.clear();
            stream << value_l;
            stream >> output;
            // std::cout << str << std::endl;
            // std::cout << output << std::endl;
            if (str != output)
            {
                return false;
            }
        }
    }
    return true;
}

/** Test suit */
void runTests() {
    std::cout << "Test run started." << std::endl;

    for (const auto& pair: COMPARISON_PAIRS)
    {
        if (!testComparison(pair.first, pair.second))
        {
            std::cout << "Failed to compare " << pair.first << " vs " << pair.second <<std::endl;
            return;
        }
        if (!testComparison(pair.second, pair.first))
        {
            std::cout << "Failed to compare " << pair.second << " vs " << pair.first <<std::endl;
            return;
        }
    }
    
    for (const auto& progression: PROGRESSIONS)
    {
        if (!testArithmeticProgression(progression.a0, progression.d, progression.n))
        {
            std::cout << "Failed to calculate arithmetic progression a0 = " << progression.a0 << " d = " << progression.d << " n = " << progression.n << std::endl;
            return;
        }
        if (!testIncrement(progression.a0, progression.n))
        {
            std::cout << "Failed to increment from value " << progression.a0 << " " << progression.n << " times"<< std::endl;
            return;
        }
    }
    
    for (const auto& value: TO_DECIMAL_TEST_VALUES)
    {
        if (!testToDecimalString(value, std::to_string(value)))
        {
            std::cout << "Failed to create decimal string presentation of " << value << std::endl;
        }
    }
    
    std::cout << "Test run completed." << std::endl;
}

int main()
{
    runTests();
    
    return 0;
}
