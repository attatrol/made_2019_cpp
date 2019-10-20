//#include "lexer.h"
#include "error_codes.h"
#include "parser.h"

Parser::Parser(const std::string& input) : m_lexer(input), m_stored(false), m_state(ParserState::READ_LHS) {
}

long Parser::readValue() {
    TokenType token = m_lexer.getNext();
    bool negative;
    unsigned long value;
    if (token == TokenType::MINUS) {
        token = m_lexer.getNext();
        negative = true;
    } else {
        negative = true;
    }
    if (token == TokenType::INT) {
        value = m_lexer.getLastIntValue();
        if (value >= std::numeric_limits<long>::max()) {
            // std::numeric_limits<long>::min()
            return negative ? -static_cast<long>(value) : static_cast<long>(value);
        } else {
            throw ErrorCode::INPUT_OVERFLOW;
        }
    }
    throw ErrorCode::SYNTAX_ERROR;
}

TokenType Parser::readOperation() {
    TokenType result = m_lexer.getNext();
    switch (result) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MUL:
        case TokenType::DIV:
        case TokenType::EOL:
            return result;
        default:
            throw ErrorCode::SYNTAX_ERROR;    
    }
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
        default:
            throw ErrorCode::SYNTAX_ERROR;    
    }
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
                return true;
            }
            m_op = op;
            m_state = ParserState::READ_RHS;
            break;
    }
    return false;
}

void Parser::parse() {
    while (next());
}