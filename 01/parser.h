//#include <limits>
#include <stack>

#include "lexer.h"

enum ParserState {
    READ_LHS,
    READ_OP_AND_PROCESS_STORED,
    READ_RHS,
    FINISHED
};

bool isHighProprityOp(TokenType op) {
    return op == TokenType::PLUS || op == TokenType::MINUS;
}

class Parser {
    private:
        Lexer m_lexer;
        ParserState m_state;
        bool m_stored;
        TokenType m_stored_op;
        long m_stored_lhs;
        TokenType m_op;
        long m_lhs;
        long m_rhs;
        long readValue();
        TokenType readOperation();
        void reduceToLhs();
        bool next();
    public:
        Parser(const std::string& input);
        void parse();
        bool getFinished() { return m_state == ParserState::FINISHED; }
        long getResult() { return m_lhs; }
};

void calcAdd(long& lhs, long rhs) {
    if (lhs > 0 ? std::numeric_limits<long>::min() + lhs > rhs : std::numeric_limits<long>::max() + lhs < rhs) {
        throw ErrorCode::OP_OVERFLOW;
    }
    lhs += rhs;
}

void calcSub(long& lhs, long rhs) {
    if (lhs > 0 ? std::numeric_limits<long>::max() - lhs < rhs : std::numeric_limits<long>::min() - lhs > rhs) {
        throw ErrorCode::OP_OVERFLOW;
    }
    lhs -= rhs;
}

void calcMul(long& lhs, long rhs) {
    if (lhs > 0 && rhs > 0 && lhs > INT64_MAX / rhs ||
        lhs < 0 && rhs > 0 && lhs < INT64_MIN / rhs ||
        lhs > 0 && rhs < 0 && rhs < INT64_MIN / lhs ||
        lhs < 0 && rhs < 0 && (lhs <= INT64_MIN || rhs <= INT64_MIN || -lhs > INT64_MAX / -rhs)) {
        throw ErrorCode::OP_OVERFLOW;    
    }
    lhs *= rhs;
}

void calcDiv(long& lhs, long rhs) {
    if (rhs == 0) {
        throw ErrorCode::DIV_BY_ZERO;
    }
    lhs /= rhs;
}