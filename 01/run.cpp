#include <iostream>

#include "parser.h"

/** Program entry point */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        return ErrorCode::NO_INPUT;
    }
    Parser parser = Parser();
    try {
        parser.setInput(argv[1]);
        parser.parse();
        std::cout << parser.getResult() << std::endl;
        return 0;
    } catch (const ParserException& e) {
        return e.errorCode;
    }
}