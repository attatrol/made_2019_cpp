#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <exception>

/**
 * Legal non-zero error codes.
 * Represent feasible errors caused by a bad input string.
 * Program expected to use one of these as a return code.
 */
enum ErrorCode {
    INPUT_OVERFLOW = 1, /* Read number is beyond domain of long type */
    UNKNOWN_TOKEN = 2,  /* Lexer encountered unknown symbol */
    OP_OVERFLOW = 3,    /* Overflow in arithmetic operation */
    DIV_BY_ZERO = 4,    /* Division by zero */
    SYNTAX_ERROR = 5,   /* Token sequence doesn't satisfy a valid expression grammar */
    NO_INPUT = 6        /* No input string */
};

/**
 * Object wrapper over ErrorCode.
 * 
 * Actually the program doesn't need this,
 * but i'm wary to throw unwrapped ErrorCode,
 * it looks like a violation of OOP.
 */
struct ParserException : public std::exception {
    const ErrorCode errorCode;

    ParserException(ErrorCode errorCode): errorCode(errorCode) { }

	const char* what() noexcept {
    	return "Parser exception";
    }
};

#endif /* ERROR_CODES_H */