#ifndef FORMATTER_H
#define FORMATTER_H

#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

/** Substitutes templates in the string with explicit values */
class Formatter
{
public:
    static const char TEMPLATE_INDEX_PREFIX = '{';  /* Template start code */
    static const char TEMPLATE_INDEX_POSTFIX = '}'; /* Template end code */
private:
    /** String split token type */
    enum TokenType {
        ARGUMENT, /* Argument */
        CONSTANT  /* Constant string */
    };
    /** String split token */
    struct Token {
        TokenType type_;    /* type */
        std::size_t index_; /* value index */
        /** Default ctor */
        Token(TokenType type, std::size_t index) : type_(type), index_(index)
        {
        }
    };
    std::vector<Token> tokens_;          /* split tokens */
    std::vector<std::string> constants_; /* constant token values */
    std::size_t argCount_;               /* argument count */
    std::vector<bool> usedArgIndexes_;   /* list of argument usage types */

    /** Parse the template string into tokens */
    void parse(const std::string& fmt);
    /** Calculate argument usage flags */
    void calcUsedArgIndexes();
    /** Translate arguments into strings, terminal case */
    template <typename T>
    void stringifyArgs(std::vector<std::string>& strArgs, T&& arg)
    {
        if (usedArgIndexes_[argCount_ - 1])
        {
            std::stringstream s;
            s << std::forward<T>(arg);
            strArgs[argCount_ - 1] = s.str();
        }
    }
    /** Translate arguments into strings */
    template <typename T, typename... Args>
    void stringifyArgs(std::vector<std::string>& strArgs, T&& arg, Args&&... args)
    {
        if (usedArgIndexes_[argCount_ - sizeof...(Args) - 1])
        {
            std::stringstream s;
            s << std::forward<T>(arg);
            strArgs[argCount_ - sizeof...(Args) - 1] = s.str();
        }
        stringifyArgs(strArgs, std::forward<Args>(args)...);
    }
public:
    /** Perform template substitution, degenerate case */
    std::string operator()()
    {
        if (argCount_)
        {
            throw std::runtime_error("Not enough arguments");
        }
        assert(constants_.size() == 1);
        return constants_[0];
    }
    /** Perform template substitution */
    template <typename... Args>
    std::string operator()(Args&&... args)
    {
        if (argCount_ > sizeof...(Args))
        {
            throw std::runtime_error("Not enough arguments");
        }

        std::vector<std::string> strArgs(argCount_);
        stringifyArgs(strArgs, std::forward<Args>(args)...);

        std::stringstream result;
        for (const Token& token : tokens_)
        {
            if (token.type_ == TokenType::CONSTANT)
            {
                result << constants_[token.index_];
            }
            else
            {
                result << strArgs[token.index_];
            }
        }

        return result.str();
    }
    /** Default ctor */
    explicit Formatter(const std::string& fmt): argCount_(0)
    {
        parse(fmt);
        calcUsedArgIndexes();
    }
    Formatter(const Formatter&) = delete;
    Formatter& operator=(const Formatter&) = delete;
    Formatter(Formatter&&) = delete;
    Formatter& operator=(Formatter&&) = delete;
};

/** Substitutes templates in the string with explicit values */
template <typename... Args>
std::string format(const std::string& fmt, Args&&... args)
{
    return Formatter(fmt)(std::forward<Args>(args)...);
}

#endif // FORMATTER_H
