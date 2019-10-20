#ifndef ERROR_CODES_H
#define ERROR_CODES_H

enum ErrorCode {
    EMPTY_INPUT = 1,
    INPUT_OVERFLOW = 2,
    UNKNOWN_TOKEN = 3,
    OP_OVERFLOW = 4,
    DIV_BY_ZERO = 5,
    SYNTAX_ERROR = 6
};

#endif /* ERROR_CODES_H */