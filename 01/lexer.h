#include <string>

enum TokenType {
    SPACE,
    EOL,
    PLUS,
    MINUS,
    MUL,
    DIV,
    INT,
    NEVER
};

class Lexer {
    private:
        const std::string& m_input;
        std::string::size_type m_charIdx;
        unsigned long long m_lastValue;
    public:
        Lexer(const std::string& input);
        TokenType getNext();
        unsigned long long getLastIntValue() { return m_lastValue; };
};
