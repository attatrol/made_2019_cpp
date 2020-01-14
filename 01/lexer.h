#ifndef LEXER_H
#define LEXER_H

#include <string>

/** Known lexemes */
enum TokenType {
    SPACE, /* whitespace between the meaningful lexemes.
            * there should be no whitespace between a unary minus and an integer.
            */
    EOL,    /* indicator of reaching the end of the input line */
    PLUS,   /* addition */
    MINUS,  /* substraction or unary minus */
    MUL,    /* multiplication */
    DIV,    /* division */
    INT     /* integer value */
};

class Lexer {
    private:
        std::string m_input;         /* the input string */
        std::size_t m_idx;           /* index of the next character to read */
        unsigned long m_lastValue;   /* last parsed integer */
    public:
        /** Default ctor */
        Lexer();
        /** Set the input and zero the char index */
        void setInput(const std::string& input);
        /** Get the next token */
        TokenType getNext();
        /** Get the last encountered integer */
        unsigned long getLastIntValue() { return m_lastValue; };
};

#endif /* LEXER_H */
