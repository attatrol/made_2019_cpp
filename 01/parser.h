#ifndef PARSER_H
#define PARSER_H

#include <limits>

#include "error_codes.h"
#include "lexer.h"

enum ParserState {
    READ_LHS,
    READ_OP,
    READ_RHS,
    READ_OP_AND_PROCESS_STORED,
    FINISHED
};

void calcAdd(long& lhs, long rhs);
void calcSub(long& lhs, long rhs);
void calcMul(long& lhs, long rhs);
void calcDiv(long& lhs, long rhs);

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
        Parser();
        void setInput(const char* input);
        void parse();
        bool getFinished() { return m_state == ParserState::FINISHED; }
        long getResult() { return m_lhs; }
};

#endif /* PARSER_H */