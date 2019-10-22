#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <exception>

/**
 * Legal non-zero error codes.
 * Represent feasible errors caused by a bad input string.
 * Program expected to use one of these as a return code.
 */
enum ErrorCode {
    INPUT_OVERFLOW = 1,
    UNKNOWN_TOKEN = 2,
    OP_OVERFLOW = 3,
    DIV_BY_ZERO = 4,
    SYNTAX_ERROR = 5,
    NO_INPUT = 6
};

/**
 * 
 */
struct ParserException : public std::exception {
    const ErrorCode errorCode;

    ParserException(ErrorCode errorCode): errorCode(errorCode) { }

	const char* what() noexcept {
    	return "Parser exception";
    }
};

#endif /* ERROR_CODES_H */