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
        const char* m_input; // no poiter ownship hence no need for destructor
        std::string::size_type m_idx;
        unsigned long m_lastValue;
    public:
        Lexer();
//        Lexer(const Lexer& other);
        void setInput(const char* input);
        TokenType getNext();
        unsigned long getLastIntValue() { return m_lastValue; };
};

#endif /* LEXER_H */
