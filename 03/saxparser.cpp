#include <limits>

#include "saxparser.h"

void SaxParser::parse(const std::string& input) {
    /* parser state */
    enum CharType
    {
        NONE,
        DIGIT,
        LETTER
    };

    m_onStart.fireEvent();
    CharType charType = CharType::NONE;
    unsigned long intValue;
    std::size_t wordStartIdx;
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        char ch = input[i];
        if (std::isspace(ch))
        {
            switch (charType)
            {
                case CharType::DIGIT:
                    m_onIntParsed.fireEvent(intValue);
                    charType = CharType::NONE;
                    break;
                case CharType::LETTER:
                    m_onWordParsed.fireEvent(input.substr(wordStartIdx, i - wordStartIdx));
                    charType = CharType::NONE;
                    break;
            }
        }
        else if (std::isdigit(ch))
        {
            switch (charType)
            {
                case CharType::NONE:
                    intValue = ch - '0';
                    charType = CharType::DIGIT;
                    break;
                case CharType::DIGIT:
                    if (intValue < (std::numeric_limits<unsigned long>::max() - 9) / 10)
                    {
                        intValue = 10 * intValue + (ch - '0');
                    }
                    else
                    {
                        if (intValue <= std::numeric_limits<unsigned long>::max() / 10)
                        {
                            intValue *= 10;
                            if (intValue <= std::numeric_limits<unsigned long>::max() - (ch - '0'))
                            {
                                intValue += ch - '0';
                            } 
                            else
                            {
                                throw ParserException(ErrorCode::INPUT_OVERFLOW);
                            }
                        }
                        else
                        {
                            throw ParserException(ErrorCode::INPUT_OVERFLOW);
                        }
                    }
                    break;
                case CharType::LETTER:
                    throw ParserException(ErrorCode::TOKENS_NOT_SEPARATED);
            }
        }
        else
        {
            switch (charType)
            {
                case CharType::NONE:
                    wordStartIdx = i;
                    charType = CharType::LETTER;
                    break;
                case CharType::DIGIT:
                    throw ParserException(ErrorCode::TOKENS_NOT_SEPARATED);
            } 
        }
    }

    switch (charType)
    {
        case CharType::DIGIT:
            m_onIntParsed.fireEvent(intValue);
            break;
        case CharType::LETTER:
            m_onWordParsed.fireEvent(input.substr(wordStartIdx, input.size() - wordStartIdx));
            break;
    }

    m_onEnd.fireEvent();
}
