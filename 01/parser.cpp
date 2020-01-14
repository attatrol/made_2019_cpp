//#include "lexer.h"
#include "error_codes.h"
#include "parser.h"

Parser::Parser(): m_lexer(), m_state(ParserState::EMPTY), m_stored(false) {
}

void Parser::setInput(const std::string& input) {
    m_lexer.setInput(input);
    m_stored = false;
    m_state = input.empty() ? ParserState::EMPTY : ParserState::READ_LHS;
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
    if (token != TokenType::INT) {
        throw ParserException(ErrorCode::SYNTAX_ERROR);
    }
    unsigned long value = m_lexer.getLastIntValue();
    if (negative) {
        if (value <= std::numeric_limits<long>::max()) {
            return -static_cast<long>(value);
        } else if (value == static_cast<unsigned long>(std::numeric_limits<long>::max()) + 1) {
            return std::numeric_limits<long>::min();
        }

    } else if (value <= std::numeric_limits<long>::max()) {
        return static_cast<long>(value);
    }
    throw ParserException(ErrorCode::INPUT_OVERFLOW);
}

TokenType Parser::readOperation() {
    TokenType result = m_lexer.getNext();
    switch (result) {
        case TokenType::SPACE:
            return readOperation(); // recursion depth is limited to 1: spaces are squashed by the lexer
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

inline void calcAdd(long& lhs, long rhs) {
    if (lhs > 0 ? std::numeric_limits<long>::min() + lhs > rhs : std::numeric_limits<long>::max() + lhs < rhs) {
        throw ParserException(ErrorCode::OP_OVERFLOW);
    }
    lhs += rhs;
}

inline void calcSub(long& lhs, long rhs) {
    if (lhs > 0 ? std::numeric_limits<long>::max() - lhs < rhs : std::numeric_limits<long>::min() - lhs > rhs) {
        throw ParserException(ErrorCode::OP_OVERFLOW);
    }
    lhs -= rhs;
}

inline void calcMul(long& lhs, long rhs) {
    if (lhs > 0 && rhs > 0 && lhs > std::numeric_limits<long>::max() / rhs ||
        lhs < 0 && rhs > 0 && lhs < std::numeric_limits<long>::min() / rhs ||
        lhs > 0 && rhs < 0 && rhs < std::numeric_limits<long>::min() / lhs ||
        lhs < 0 && rhs < 0 && (lhs < std::numeric_limits<long>::min() || rhs < std::numeric_limits<long>::min() || -lhs > std::numeric_limits<long>::max() / -rhs)) {
        throw ParserException(ErrorCode::OP_OVERFLOW);
    }
    lhs *= rhs;
}

inline void calcDiv(long& lhs, long rhs) {
    if (rhs == 0) {
        throw ParserException(ErrorCode::DIV_BY_ZERO);
    }
    if (lhs == std::numeric_limits<long>::min() && rhs == -1L) {
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

inline bool isHighProprityOp(TokenType op) {
    return op == TokenType::MUL || op == TokenType::DIV;
}

void Parser::parse() {
    while (m_state != ParserState::FINISHED) {
        switch (m_state) {
            case ParserState::EMPTY:
                throw ParserException(ErrorCode::NO_INPUT);    
            case ParserState::READ_LHS:
                m_lhs = readValue();
                m_state = ParserState::READ_OP;
                break;
            case ParserState::READ_OP:
                m_op = readOperation();
                m_state = m_op == TokenType::EOL ? ParserState::FINISHED : ParserState::READ_RHS;
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
                    if (m_stored && !isHighProprityOp(op)) {
                        reduceToLhs();
                        // pop
                        m_rhs = m_lhs;
                        m_lhs = m_stored_lhs;
                        m_op = m_stored_op;
                        m_stored = false;
                    }
                    reduceToLhs();
                }
                m_state = op == TokenType::EOL ? ParserState::FINISHED : ParserState::READ_RHS;
                m_op = op;
                break;
        }
    }
}
