#include <limits>

#include "error_codes.h"
#include "lexer.h"

Lexer::Lexer(): m_input(nullptr), m_idx(0) {
}

void Lexer::setInput(const char* input) {
    m_input = input;
    m_idx = 0;
}

TokenType Lexer::getNext() { 
    char letter = m_input[m_idx++];
    if (letter == '\0')
        return TokenType::EOL;

    // TokenType::SPACE
    if (std::isspace(letter)) {
        while (std::isspace(letter = m_input[m_idx])) {
            m_idx++;
        }
        return TokenType::SPACE;
    }

    // TokenType::INT
    if (std::isdigit(letter)) {
        m_lastValue = letter - '0';
        while (std::isdigit(letter = m_input[m_idx])) {
            if (m_lastValue < (std::numeric_limits<unsigned long>::max() - 9) / 10) {
                m_lastValue = 10 * m_lastValue + (letter - '0');
            } else {
                if (m_lastValue <= std::numeric_limits<unsigned long>::max() / 10) {
                        m_lastValue *= 10;
                    if (m_lastValue <= std::numeric_limits<unsigned long>::max() - (letter - '0')) {
                        m_lastValue += letter - '0';
                    } else {
                        throw ParserException(ErrorCode::INPUT_OVERFLOW);
                    }
                } else {
                    throw ParserException(ErrorCode::INPUT_OVERFLOW);
                }
            }
            m_idx++;
        }
        return TokenType::INT;
    }

    switch(letter) {
        case '+': return TokenType::PLUS;
        case '-': return TokenType::MINUS;
        case '*': return TokenType::MUL;
        case '/': return TokenType::DIV;
        default: throw ParserException(ErrorCode::UNKNOWN_TOKEN);
    }
}
