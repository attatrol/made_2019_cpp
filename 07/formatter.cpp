#include "formatter.h"

inline void throwParseException(std::size_t index)
{
    throw std::runtime_error("Faled to parse the input string on " + std::to_string(index) + " position");
}

void Formatter::parse(const std::string &fmt)
{
    /* parser state */
    enum class CharType
    {
        NONE,
        CONSTANT,
        TEMPLATE_INDEX_PREFIX,
        TEMPLATE_INDEX_POSTFIX,
        TEMPLATE_INDEX
    };

    CharType chType = CharType::NONE;
    std::size_t subStringBeginIndex;
    for (std::size_t i = 0; i < fmt.size(); ++i)
    {
        const char ch = fmt[i];

        if (ch == Formatter::TEMPLATE_INDEX_PREFIX)
        {
            switch (chType)
            {
            case CharType::NONE:
            case CharType::TEMPLATE_INDEX_POSTFIX:
                break;
            case CharType::CONSTANT:
                constants_.push_back(fmt.substr(subStringBeginIndex, i - subStringBeginIndex));
                tokens_.emplace_back(TokenType::CONSTANT, constants_.size() - 1);
                break;
            case CharType::TEMPLATE_INDEX_PREFIX:
            case CharType::TEMPLATE_INDEX:
                throwParseException(i);
                break;
            }
            chType = CharType::TEMPLATE_INDEX_PREFIX;
        }
        else if (ch == Formatter::TEMPLATE_INDEX_POSTFIX)
        {
            switch (chType)
            {
            case CharType::TEMPLATE_INDEX:
                std::size_t argIndex;
                try
                {
                    argIndex = std::stoul(fmt.substr(subStringBeginIndex, i - subStringBeginIndex));
                    if (argIndex + 1 > argCount_)
                    {
                        argCount_ = argIndex + 1;
                    }
                }
                catch (std::logic_error& e)
                {
                    throwParseException(i);
                }
                tokens_.emplace_back(TokenType::ARGUMENT, argIndex);
                break;
            case CharType::CONSTANT:
            case CharType::NONE:
            case CharType::TEMPLATE_INDEX_PREFIX:
            case CharType::TEMPLATE_INDEX_POSTFIX:
                throwParseException(i);
                break;
            }
            chType = CharType::TEMPLATE_INDEX_POSTFIX;
        }
        else
        {
            switch (chType)
            {
            case CharType::CONSTANT:
            case CharType::TEMPLATE_INDEX:
                break;
            case CharType::NONE:
            case CharType::TEMPLATE_INDEX_POSTFIX:
                subStringBeginIndex = i;
                chType = CharType::CONSTANT;
                break;

            case CharType::TEMPLATE_INDEX_PREFIX:
                subStringBeginIndex = i;
                chType = CharType::TEMPLATE_INDEX;
                break;
            }
        }
    }

    switch (chType)
    {
    case CharType::NONE:
        constants_.push_back("");
        tokens_.emplace_back(TokenType::CONSTANT, constants_.size() - 1);
        break;
    case CharType::CONSTANT:
        constants_.push_back(fmt.substr(subStringBeginIndex, fmt.size() - subStringBeginIndex));
        tokens_.emplace_back(TokenType::CONSTANT, constants_.size() - 1);
        break;
    case CharType::TEMPLATE_INDEX:
    case CharType::TEMPLATE_INDEX_PREFIX:
        throwParseException(fmt.size() - 1);
        break;
    }
}

void Formatter::calcUsedArgIndexes()
{
    usedArgIndexes_.resize(argCount_);
    for (auto it  = tokens_.begin(); it < tokens_.end(); ++it)
    {
        if (it->type_ == TokenType::ARGUMENT)
        {
            usedArgIndexes_[it->index_] = true;
        }
    }
}
