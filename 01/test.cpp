#include <iostream>
#include <sstream>

#include "parser.h"

bool testNullInput(Parser& parser) {
    if (parser.getFinished()) {
        return false;
    }
    try {
        parser.parse();
        return false;
    } catch (const ParserException& e) {
        return e.errorCode == ErrorCode::NO_INPUT;
    }
}

/* Test arithmetic operation expecting positive outcome */
bool passArithmeticOperation(void (*const op)(long& lhs, long rhs), long lhs, const long rhs, const long result) {
    try {
        op(lhs, rhs);
        return lhs == result;
    } catch (const ParserException& e) {
        return false;
    }
}

/* Test arithmetic operation expecting error */
bool failArithmeticOperation(void (*const op)(long& lhs, long rhs), long lhs, const long rhs, ErrorCode errorCode = ErrorCode::OP_OVERFLOW) {
    try {
        op(lhs, rhs);
        return false;
    } catch (const ParserException& e) {
        return e.errorCode == errorCode;
    }
}

/* Test addition as utility arithmetic operation */
bool testAdd() {
    return
        passArithmeticOperation(calcAdd, 1, 0, 1) ||
        passArithmeticOperation(calcAdd, 1, -1, 0) ||
        passArithmeticOperation(calcAdd, std::numeric_limits<long>::max(), std::numeric_limits<long>::min(), -1) ||
        passArithmeticOperation(calcAdd, std::numeric_limits<long>::min() + 3L, std::numeric_limits<long>::max(), 2) ||
        passArithmeticOperation(calcAdd, std::numeric_limits<long>::max() / 2, std::numeric_limits<long>::max() / 2 + 1, std::numeric_limits<long>::max()) ||
        passArithmeticOperation(calcAdd, std::numeric_limits<long>::min() / 2, std::numeric_limits<long>::min() / 2, std::numeric_limits<long>::min()) ||
        passArithmeticOperation(calcAdd, 123, -223, -100) ||

        failArithmeticOperation(calcAdd, std::numeric_limits<long>::max(), 1) ||
        failArithmeticOperation(calcAdd, std::numeric_limits<long>::max(), std::numeric_limits<long>::max()) ||
        failArithmeticOperation(calcAdd, std::numeric_limits<long>::min(), std::numeric_limits<long>::min()) ||
        failArithmeticOperation(calcAdd, std::numeric_limits<long>::min(), -1) ||
        failArithmeticOperation(calcAdd, std::numeric_limits<long>::max() / 2 + 1, std::numeric_limits<long>::max() / 2 + 1) ||
        failArithmeticOperation(calcAdd, std::numeric_limits<long>::max() - 1000, 2000) ||
        failArithmeticOperation(calcAdd, std::numeric_limits<long>::min() / 2, std::numeric_limits<long>::min() / 2 - 1);
}

/* Test substraction as utility arithmetic operation */
bool testSub() {
    return
        passArithmeticOperation(calcSub, 1, 0, 1) ||
        passArithmeticOperation(calcSub, 1, -1, 2) ||
        passArithmeticOperation(calcSub, std::numeric_limits<long>::max(), std::numeric_limits<long>::max(), 0L) ||
        passArithmeticOperation(calcSub, std::numeric_limits<long>::max() / 2, std::numeric_limits<long>::min() / 2, std::numeric_limits<long>::max()) ||
        passArithmeticOperation(calcSub, std::numeric_limits<long>::max() / 2, std::numeric_limits<long>::max() / 2 + 1, -1) ||
        passArithmeticOperation(calcSub, std::numeric_limits<long>::min() / 2, std::numeric_limits<long>::min() / -2, std::numeric_limits<long>::min()) ||
        passArithmeticOperation(calcSub, 123L, -100L, 223L) ||

        failArithmeticOperation(calcSub, std::numeric_limits<long>::max(), -1) ||
        failArithmeticOperation(calcSub, std::numeric_limits<long>::max(), std::numeric_limits<long>::min() + 1) ||
        failArithmeticOperation(calcSub, std::numeric_limits<long>::min(), std::numeric_limits<long>::max()) ||
        failArithmeticOperation(calcSub, std::numeric_limits<long>::min(), 1) ||
        failArithmeticOperation(calcSub, std::numeric_limits<long>::max() / -2 - 1, std::numeric_limits<long>::max() / -2 - 1) ||
        failArithmeticOperation(calcSub, std::numeric_limits<long>::max() - 1000, -2000) ||
        failArithmeticOperation(calcSub, std::numeric_limits<long>::min() / 2, std::numeric_limits<long>::min() / -2 + 1);
}

/* Test multiplication as utility arithmetic operation */
bool testMul() {
    return
        passArithmeticOperation(calcMul, 1, 0, 0) ||
        passArithmeticOperation(calcMul, 2, -2, -4) ||
        passArithmeticOperation(calcMul, std::numeric_limits<long>::max() / 2, 2, std::numeric_limits<long>::max() - 1) ||
        passArithmeticOperation(calcMul, std::numeric_limits<long>::max() / 2, -2, std::numeric_limits<long>::min() + 1) ||
        passArithmeticOperation(calcMul, std::numeric_limits<long>::min() / 2, 2, std::numeric_limits<long>::min()) ||
        passArithmeticOperation(calcMul, 12, -12, -144) ||

        failArithmeticOperation(calcMul, std::numeric_limits<long>::min(), -1) ||
        failArithmeticOperation(calcMul, std::numeric_limits<long>::max(), 2) ||
        failArithmeticOperation(calcMul, std::numeric_limits<long>::min(), std::numeric_limits<long>::max()) ||
        failArithmeticOperation(calcMul, std::numeric_limits<long>::min(), std::numeric_limits<long>::min()) ||
        failArithmeticOperation(calcMul, std::numeric_limits<long>::max(), std::numeric_limits<long>::max()) ||
        failArithmeticOperation(calcMul, std::numeric_limits<long>::max() - 1000, -2000) ||
        failArithmeticOperation(calcMul, std::numeric_limits<long>::min() / 2, std::numeric_limits<long>::min() / -2 + 1);
}

/* Test division as utility arithmetic operation */
bool testDiv() {
    return
        passArithmeticOperation(calcDiv, -42, -1, 42) ||
        passArithmeticOperation(calcDiv, 2, -2, -1) ||
        passArithmeticOperation(calcDiv, std::numeric_limits<long>::max() / 2, 2, std::numeric_limits<long>::max() / 4) ||
        passArithmeticOperation(calcDiv, std::numeric_limits<long>::max() / 2, -2, std::numeric_limits<long>::max() / -4) ||
        passArithmeticOperation(calcDiv, std::numeric_limits<long>::min(), std::numeric_limits<long>::max(), -1) ||
        passArithmeticOperation(calcDiv, 120, -12, -10) ||

        failArithmeticOperation(calcDiv, std::numeric_limits<long>::min(), -1) ||
        failArithmeticOperation(calcDiv, std::numeric_limits<long>::max(), 0, ErrorCode::DIV_BY_ZERO) ||
        failArithmeticOperation(calcDiv, -42, -0, ErrorCode::DIV_BY_ZERO);
}

/* Test the domain errors during conversion of chars into an integer */
bool testIntDomain(Parser& parser) {
    // first test for values that lie near the domain border
    try {
        for (long i = 0; i <= 11; i++) {
            const std::string max = std::to_string(std::numeric_limits<long>::max() - i);
            parser.setInput(max);
            parser.parse();
            if (!parser.getFinished() || parser.getResult() != std::numeric_limits<long>::max() - i) {
                return false;
            }
            const std::string min = std::to_string(std::numeric_limits<long>::min() + i);
            parser.setInput(min);
            parser.parse();
            if (!parser.getFinished() || parser.getResult() != std::numeric_limits<long>::min() + i) {
                return false;
            }
        }
    } catch (const ParserException& e) {
        return false;
    }
    // then check those beyond the domain of long
    for (long i = 0; i <= 11; i++) {
        try {
            const std::string max = std::to_string(std::numeric_limits<unsigned long>::max() / 2 + 1 + i);
            parser.setInput(max);
            parser.parse();
            return false;
        } catch (const ParserException& e) {
            if (e.errorCode != ErrorCode::INPUT_OVERFLOW) {
                return false;
            }
        }
        try {
            const std::string min = "-" + std::to_string(std::numeric_limits<unsigned long>::max() / 2 + 2 + i);
            parser.setInput(min);
            parser.parse();
            return false;
        } catch (const ParserException& e) {
            if (e.errorCode != ErrorCode::INPUT_OVERFLOW) {
                return false;
            }
        }
    }
    return true;
}

/** A test case for the parser run test */
struct ExpressionTestCase {
    const std::string name;          /* case name */
    const std::string input;         /* input string */
    const bool success;              /* expected outcome */
    union {
        const ErrorCode errorCode;   /* error code */
        const long value;            /* return value */
    } result;
};

/* Collection of various expressions */
const  ExpressionTestCase FIXTURES[] = {
    { "Empty input-0", "",       false, ErrorCode::NO_INPUT },
    { "Empty input-1", "   ",    false, ErrorCode::SYNTAX_ERROR },

    { "Simple value-0", "0",      true, { .value = 0L } },
    { "Simple value-1", "-0",     true, { .value = 0L } },
    { "Simple value-2", "  1",    true, { .value = 1L } },
    { "Simple value-3", "-1 ",    true, { .value = -1L } },
    { "Simple value-4", "  42  ", true, { .value = 42L } },
    { "Simple value-5", "-20000", true, { .value = -20000L } },

    { "No space between number and unary minus", "- 20000", false, ErrorCode::SYNTAX_ERROR },
    { "Number after number-0",       "100 100",             false, ErrorCode::SYNTAX_ERROR },
    { "Number after number-1",       "-100 100    ",        false, ErrorCode::SYNTAX_ERROR },
    { "Operation after operation-0", "100 -- 100",          false, ErrorCode::SYNTAX_ERROR },
    { "Operation after operation-1", "100 - - 100",         false, ErrorCode::SYNTAX_ERROR },
    { "Operation after operation-2", "  100 *- 100",        false, ErrorCode::SYNTAX_ERROR },
    { "Operation after operation-3", "100++100 ",           false, ErrorCode::SYNTAX_ERROR },
    { "Operation after operation-4", "   100-*100",         false, ErrorCode::SYNTAX_ERROR },
    { "Operation after operation-5", "100/*100",            false, ErrorCode::SYNTAX_ERROR },
    { "Starting with operation-0",   "*100 - 100",          false, ErrorCode::SYNTAX_ERROR },
    { "Starting with operation-1",   "+100",                false, ErrorCode::SYNTAX_ERROR },
    { "Trailing operation-0",        "  100 -",             false, ErrorCode::SYNTAX_ERROR },
    { "Trailing operation-1",        "2*-2*",               false, ErrorCode::SYNTAX_ERROR },
    { "Unknown token-0",             "20 - 7,7",            false, ErrorCode::UNKNOWN_TOKEN },
    { "Unknown token-1",             "2 x 3",               false, ErrorCode::UNKNOWN_TOKEN },
    { "Unknown token-1",             "rgrtu",               false, ErrorCode::UNKNOWN_TOKEN },

    { "Operation combos-0", "8 / 2 * 3 --5",               true, { .value = 8L / 2L * 3L - -5L } },
    { "Operation combos-1", "8 / 2 + 3 --5",               true, { .value = 8L / 2L + 3L - -5L } },
    { "Operation combos-2", "8 - 2 * 3 *-5",               true, { .value = 8L - 2L * 3L * -5L } },
    { "Operation combos-3", "1 - 8 / 2 * 3 --5",           true, { .value = 1L - 8L / 2L * 3L - -5L } },
    { "Operation combos-4", "33 * 44 /12  -8 / 2 * 3 --5", true, { .value = 33L * 44L / 12L - 8L / 2L * 3L - -5L } },
    { "Operation combos-5", "8 - -2 * 3 --5",              true, { .value = 8L - -2L * 3L - -5L } },
    { "Operation combos-6", "8 / 2 + 3 /-5",               true, { .value = 8L / 2L + 3L / -5L } },

    { "Pass-0", "1+2+3+4+5+6+7+8+9+10", true, { .value = 55L } },
    { "Pass-1", "-234*44  - -6 / 2", true, { .value = -234L * 44L - -6L / 2L } },
    { "Pass-2", "100000 / 33 / 12 - 19 / 9 * 6--98", true, { .value = 100000L / 33L / 12L - 19L / 9L * 6L - -98L } },
    { "Pass-3", "1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 *10 /9/8/7 /6 / 5 /-4 /3 / 2 / 1", true, { .value = -10L } },
    { "Pass-4", "-100000 / -2 + 1 / 1000000", true, { .value = -100000L / -2L + 1L / 1000000L } },
    { "Pass-5", "   7812*9259- -545 * -42433 + -4 * -4  / 2 - -132424 / 432 ", true,
        { .value = 7812L * 9259L - -545L * -42433L + -4L * -4L / 2L - -132424L / 432L } }
};

/** Test an expression parsing */
bool runExpressionTest(Parser& parser, const ExpressionTestCase& fixture) {
    try {
        parser.setInput(fixture.input);
        parser.parse();
        return fixture.success && parser.getFinished() && parser.getResult() == fixture.result.value;
    } catch (const ParserException& e) {
        return !fixture.success && e.errorCode == fixture.result.errorCode;
    }
}

/** Test suit */
void runTests() {
    std::cout << "Test run started." << std::endl;
    Parser parser = Parser();
    if (!testNullInput(parser))
        std::cout << "Failed to perform without input" << std::endl;
    if (!testAdd())
        std::cout << "Addition tests failed" << std::endl;
    if (!testSub())
        std::cout << "Substraction tests failed" << std::endl;
    if (!testMul())
        std::cout << "Multiplication tests failed" << std::endl;
    if (!testDiv())
        std::cout << "Division tests failed" << std::endl;
    if(!testIntDomain(parser))
        std::cout << "Integer domain tests failed" << std::endl;
    for (ExpressionTestCase fixture : FIXTURES)
        if(!runExpressionTest(parser, fixture))
            std::cout << "Expression test \"" << fixture.name << "\" failed." << std::endl;
    std::cout << "Test run completed." << std::endl;
}

/** Program entry point */
int main(void) {
    runTests();
}
