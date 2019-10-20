#ifndef LEXER_H
#define LEXER_H

#include <string>

enum TokenType {
    SPACE,
    EOL,
    PLUS,
    MINUS,
    MUL,
    DIV,
    INT
};

class Lexer {
    private:
        const std::string& m_input;
        std::string::size_type m_charIdx;
        unsigned long m_lastValue;
    public:
        Lexer(const std::string& input);
        TokenType getNext();
        unsigned long getLastIntValue() { return m_lastValue; };
};

#endif /* LEXER_H */
