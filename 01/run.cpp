#include <iostream>

#include "parser.h"

/** Program entry point */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        return ErrorCode::NO_INPUT;
    }
    std::string input(argv[1]);
    Parser parser = Parser();
    try {
        parser.setInput(input);
        parser.parse();
        std::cout << parser.getResult() << std::endl;
        return 0;
    } catch (const ParserException& e) {
        return e.errorCode;
    }
}
