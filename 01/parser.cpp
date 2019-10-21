//#include "lexer.h"
#include "error_codes.h"
#include "parser.h"

Parser::Parser(): m_lexer(Lexer()), m_state(ParserState::READ_LHS), m_stored(false) {
}

void Parser::setInput(const char* input) {
    m_lexer.setInput(input);
    m_stored = false;
    m_state = ParserState::READ_LHS;
}

long Parser::readValue() {
    TokenType token = m_lexer.getNext();
    if (token == TokenType::SPACE) {
        token = m_lexer.getNext();
    }
    bool negative = token == TokenType::MINUS;
    if (negative) {
        token = m_lexer.getNext();
    }
    unsigned long value;
    if (token == TokenType::INT) {
        value = m_lexer.getLastIntValue();
        if (negative) {
            if (value <= std::numeric_limits<long>::max()) {
                return -static_cast<long>(value);
            } else if (value == static_cast<unsigned long>(std::numeric_limits<long>::max()) + 1) {
                return std::numeric_limits<long>::min();
            }

        } else if (value <= std::numeric_limits<long>::max()) {
            return static_cast<long>(value);
        }
    } else {
        throw ParserException(ErrorCode::INPUT_OVERFLOW);
    }
    throw ParserException(ErrorCode::SYNTAX_ERROR);
}

TokenType Parser::readOperation() {
    TokenType result = m_lexer.getNext();
    switch (result) {
        case TokenType::SPACE:
            return readOperation(); // recursion depth is limited to 1
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MUL:
        case TokenType::DIV:
        case TokenType::EOL:
            return result;
        default:
            throw ParserException(ErrorCode::SYNTAX_ERROR);
    }
}

void calcAdd(long& lhs, long rhs) {
    if (lhs > 0 ? std::numeric_limits<long>::min() + lhs > rhs : std::numeric_limits<long>::max() + lhs < rhs) {
        throw ParserException(ErrorCode::INPUT_OVERFLOW);
    }
    lhs += rhs;
}

void calcSub(long& lhs, long rhs) {
    if (lhs > 0 ? std::numeric_limits<long>::max() - lhs < rhs : std::numeric_limits<long>::min() - lhs > rhs) {
        throw ParserException(ErrorCode::INPUT_OVERFLOW);
    }
    lhs -= rhs;
}

void calcMul(long& lhs, long rhs) {
    if (lhs > 0 && rhs > 0 && lhs > std::numeric_limits<long>::max() / rhs ||
        lhs < 0 && rhs > 0 && lhs < std::numeric_limits<long>::min() / rhs ||
        lhs > 0 && rhs < 0 && rhs < std::numeric_limits<long>::min() / lhs ||
        lhs < 0 && rhs < 0 && (lhs <= std::numeric_limits<long>::min() || rhs <= std::numeric_limits<long>::min() || -lhs > std::numeric_limits<long>::max() / -rhs)) {
        throw ParserException(ErrorCode::OP_OVERFLOW);
    }
    lhs *= rhs;
}

void calcDiv(long& lhs, long rhs) {
    if (rhs == 0) {
        throw ParserException(ErrorCode::OP_OVERFLOW);
    }
    lhs /= rhs;
}

void Parser::reduceToLhs() {
    switch (m_op) {
        case TokenType::PLUS:
            calcAdd(m_lhs, m_rhs);
            return;
        case TokenType::MINUS:
            calcSub(m_lhs, m_rhs);
            return;
        case TokenType::MUL:
            calcMul(m_lhs, m_rhs);
            return;
        case TokenType::DIV:
            calcDiv(m_lhs, m_rhs);
            return;
    }
    // assert(false);
}

bool isHighProprityOp(TokenType op) {
    return op == TokenType::PLUS || op == TokenType::MINUS;
}

bool Parser::next() {
    switch (m_state) {
        case ParserState::READ_LHS:
            m_lhs = readValue();
            m_state = ParserState::READ_OP_AND_PROCESS_STORED;
            break;
        case ParserState::READ_RHS:
            m_rhs = readValue();
            m_state = ParserState::READ_OP_AND_PROCESS_STORED;
            break;
        case ParserState::READ_OP_AND_PROCESS_STORED:
            TokenType op = readOperation();
            if (!isHighProprityOp(m_op)) {
                if (isHighProprityOp(op)) {
                    // push
                    m_stored = true;
                    m_stored_lhs = m_lhs;
                    m_stored_op = m_op;
                    m_lhs = m_rhs;
                } else {
                    reduceToLhs();
                }
            } else {
                if (m_stored) {
                    reduceToLhs();
                    // pop
                    m_rhs = m_lhs;
                    m_lhs = m_stored_lhs;
                    m_op = m_stored_op;
                    m_stored = false;
                }
                reduceToLhs();
            }
            if (op == TokenType::EOL) {
                // assert(!m_stored);
                m_state = ParserState::FINISHED;
                return false;
            }
            m_op = op;
            m_state = ParserState::READ_RHS;
            break;
    }
    return true;
}

void Parser::parse() {
    while (next());
}
