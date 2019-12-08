#include <iostream>
#include <sstream>

#include "formatter.h"

enum TestResult
{
    SUCCESS,
    FAILURE,
    EXCEPTION
};

template <typename... Args>
bool test(TestResult result, std::string expectedStr, std::string templateStr, Args&&... args)
{
    try
    {
        std::string observedStr = format(templateStr, std::forward<Args>(args)...);
        return (observedStr == expectedStr ? TestResult::SUCCESS : TestResult::FAILURE) == result;
    }
    catch (std::runtime_error&)
    {
        return TestResult::EXCEPTION == result;
    }

}

int main()
{
    std::cout << "Begin test run" << std::endl;

    if (!(test(TestResult::SUCCESS, "", "") ||
          test(TestResult::SUCCESS, "  ", "  ") ||
          test(TestResult::SUCCESS, "ABBA", "ABBA")))
    {
        std::cout << "No args test failed" << std::endl;
    }

    if (!(test(TestResult::SUCCESS, "0", "{0}", 0) ||
          test(TestResult::SUCCESS, "attatrol 1", "{0} {1}", "attatrol", 1.0) ||
          test(TestResult::SUCCESS, "01ABBA 1 0 10", "{0}{1}ABBA {1} {0} {1}{0}", 0, 1)))
    {
        std::cout << "Simple args test failed" << std::endl;
    }

    if (!(test(TestResult::SUCCESS, "0", "{1}", 42, 0) ||
          test(TestResult::SUCCESS, "-1 -1", "{2} {2}", "attatrol", -6, -1.0)))
    {
        std::cout << "Missing args test failed" << std::endl;
    }

    if (!(test(TestResult::EXCEPTION, "", "{1}", 42) ||
          test(TestResult::EXCEPTION, "", "{0} {2}", "attatrol", -6, -1.0)))
    {
        std::cout << "Not enough args test failed" << std::endl;
    }

    if (!(test(TestResult::EXCEPTION, "", "{", 0) ||
          test(TestResult::EXCEPTION, "", "{}", 0) ||
          test(TestResult::EXCEPTION, "", "{a}", 0) ||
          test(TestResult::EXCEPTION, "", "{-1}", 0) ||
          test(TestResult::EXCEPTION, "", "{{", 0) ||
          test(TestResult::EXCEPTION, "", "{{0}}", 0) ||
          test(TestResult::EXCEPTION, "", "{0", 0) ||
          test(TestResult::EXCEPTION, "", "0}", 0) ||
          test(TestResult::EXCEPTION, "", "{ 0 }", 0)))
    {
        std::cout << "Parse error test failed" << std::endl;
    }

    if (!(test(TestResult::EXCEPTION, "", "{99999999999999999999999999999999999}", 42)))
    {
        std::cout << "Arg index overflow test failed" << std::endl;
    }

    std::cout << "End test run" << std::endl;

    return 0;
}
