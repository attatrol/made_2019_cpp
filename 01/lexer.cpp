#include <limits>

#include "error_codes.h"
#include "lexer.h"

Lexer::Lexer(const std::string& input): m_input(input), m_charIdx(0) {}

TokenType Lexer::getNext() { 
    if (m_input.size() >= m_charIdx)
        return TokenType::EOL;
    char letter = m_input[m_charIdx++];

    // TokenType::SPACE
    if (std::isspace(letter)) {
        while (m_input.size() >= m_charIdx && std::isspace(letter = m_input[m_charIdx])) {
            m_charIdx++;
        }
        return TokenType::SPACE;
    }

    // TokenType::INT
    if (std::isdigit(letter)) {
        m_lastValue = letter - '0';
        while (m_input.size() >= m_charIdx && std::isdigit(letter = m_input[m_charIdx])) {
            if (m_lastValue < (std::numeric_limits<unsigned long>::max() - 9) / 10) {
                m_lastValue = 10 * m_lastValue + (letter - '0');
            } else {
                if (m_lastValue <= std::numeric_limits<unsigned long>::max() / 10) {
                        m_lastValue *= 10;
                    if (m_lastValue <= std::numeric_limits<unsigned long>::max() - (letter - '0')) {
                        m_lastValue += letter - '0';
                    } else {
                        throw ErrorCode::INPUT_OVERFLOW;
                    }
                } else {
                    throw ErrorCode::INPUT_OVERFLOW;
                }
            }
            m_charIdx++;
        }
        return TokenType::INT;
    }

    switch(m_input[m_charIdx]) {
        case '+': return TokenType::PLUS;
        case '-': return TokenType::MINUS;
        case '*': return TokenType::MUL;
        case '/': return TokenType::DIV;
        default: throw ErrorCode::UNKNOWN_TOKEN;
    }
}
