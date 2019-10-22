#ifndef PARSER_H
#define PARSER_H

#include <limits>

#include "error_codes.h"
#include "lexer.h"

/** 
 * Internal parser state.
 * Describes an action parser should perform on the next step.
 */
enum ParserState {
    EMPTY,    /* input not present */
    READ_LHS, /* read left operand */
    READ_OP,  /* read operation */
    READ_RHS, /* read right operand */
    READ_OP_AND_PROCESS_STORED, /* read operation then try either to store operation 
                                 * or to process already store operation depending on the operations' priority */
    FINISHED /* finite state */
};

/**
 * Simple arithmetical routines that also perform domain checks.
 */
void calcAdd(long& lhs, long rhs); /* addition */
void calcSub(long& lhs, long rhs); /* substraction */
void calcMul(long& lhs, long rhs); /* multiplication */
void calcDiv(long& lhs, long rhs); /* division */

/**
 * The parser.
 * Calcutates arithmetical expression in form of a string.
 */
class Parser {
    private:
        Lexer m_lexer;          /* lexer */
        ParserState m_state;    /* internal state */
        bool m_stored;          /* flag of occupied storage fields.
                                 * for the sake of simplicity I decided to use variables instead of a stack.
                                 * if there were more than 2 levels of operation priority or some brackets
                                 * then a stack is a preferred option/
                                 */
        TokenType m_stored_op;  /* stored low priority operation */
        long m_stored_lhs;      /* stored left operand */
        TokenType m_op;         /* current operation */
        long m_lhs;             /* left operand */
        long m_rhs;             /* right operand */

        /** Pull the next token as an integer value from the parser */
        long readValue();
        /** Pull the next token as an operation from the parser */
        TokenType readOperation();
        /** Perform the current operation over lhs & rhs and store the result into lhs */
        void reduceToLhs();
        /** Pull the next token as an operation from the parser */
        bool next();
    public:
        /** Default ctor */
        Parser();
        /** Set input string */
        void setInput(const char* input);
        /** Parse input string */
        void parse();
        /** Flag that indicates that the result is ready */
        bool getFinished() { return m_state == ParserState::FINISHED; }
        /** Parsing result */
        long getResult() { return m_lhs; }
};

#endif /* PARSER_H */