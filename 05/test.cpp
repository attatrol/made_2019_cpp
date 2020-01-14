#include <iostream>
#include <sstream>

#include "error.h"
#include "serializer.h"
#include "deserializer.h"

/** POD for tests */
struct Data
{
    uint64_t a;
    bool b;
    uint64_t c;

    template <class Serializer>
    Error serialize(Serializer& serializer)
    {
        return serializer(a, b, c);
    }
};

enum TestCycle {
    SD, /** First serialize, then deserialize */
    DS, /** First deserialize, then serialize */
    Both /** Do both SDS and DSD */
};

/** A test case for the parser run test */
struct TestCase
{
    const std::string name;
    const Data data;
    const std::string archive;
    const Error result;
    const TestCycle cycle;
};

/* Collection of various expressions */
const TestCase FIXTURES[] = {
    { "Good - 1", {0, false, 0}, "0 false 0", Error::NoError, TestCycle::Both },
    { "Good - 2", {42, false, 22222}, "42 false 22222", Error::NoError, TestCycle::Both },
    { "Good - 3", {0, true, 0}, "0 true 0", Error::NoError, TestCycle::Both },
    { "Good - 4", {1000000000, false, 0}, "1000000000 false 0", Error::NoError, TestCycle::Both },
    { "Good - 5", {0, true, 1000000000}, "0 true 1000000000", Error::NoError, TestCycle::Both },
    { "Incomplete archive - 1", {0, false, 0}, "100000000", Error::CorruptedArchive, TestCycle::DS },
    { "Incomplete archive - 2", {0, false, 0}, "100000000", Error::CorruptedArchive, TestCycle::DS },
    { "Overflow", {0, true, 0}, "0 true 5555555555555555555555555", Error::CorruptedArchive, TestCycle::DS }
};

bool testSD(Serializer& serializer, Deserializer& deserializer, std::stringstream& stream, TestCase& testCase)
{
    Data tempData = testCase.data;
    Error result = serializer.save(tempData);
    if (result != Error::NoError)
    {
        return result == testCase.result;
    }


    result = deserializer.load(tempData);
    if (result != Error::NoError)
    {
        return result == testCase.result;
    }

    stream.clear();
    stream.str(std::string());

    return tempData.a == testCase.data.a && tempData.b == testCase.data.b && tempData.c == testCase.data.c;
}

bool testDS(Serializer& serializer, Deserializer& deserializer, std::stringstream& stream, TestCase& testCase)
{
    stream.str(testCase.archive);
    Data tempData;
    Error result = deserializer.load(tempData);
    if (result != Error::NoError)
    {
        return result == testCase.result;
    }

    result = serializer.save(tempData);
    if (result != Error::NoError)
    {
        return result == testCase.result;
    }

    std::string tempArchive = stream.str();
    stream.clear();
    stream.str(std::string());

    return tempArchive == testCase.archive;
}

bool runTest(Serializer& serializer, Deserializer& deserializer, std::stringstream& stream, TestCase& testCase)
{
    switch (testCase.cycle)
    {
        case TestCycle::DS:
            return testSD(serializer, deserializer, stream, testCase);
        case TestCycle::SD:
            return testDS(serializer, deserializer, stream, testCase);
        case TestCycle::Both:
            return testSD(serializer, deserializer, stream, testCase) && testDS(serializer, deserializer, stream, testCase);
    }
    return false;
}

/** Test suit */
void runTests() {
    std::cout << "Test run started." << std::endl;

    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);

    for (TestCase fixture : FIXTURES)
    {
        if(!runTest(serializer, deserializer, stream, fixture))
        {
            std::cout << "Case \"" << fixture.name << "\" failed." << std::endl;
        }
    }
    std::cout << "Test run completed." << std::endl;
}

/** Program entry point */
int main(void) {
    runTests();
}








