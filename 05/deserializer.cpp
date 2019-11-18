#include <limits>

#include "deserializer.h"

template <>
void Deserializer::process<uint64_t>(Error& result, uint64_t& val)
{
    std::string text;
    m_in >> text;

    uint64_t intVal = 0;

    for (std::size_t i = 0, length = text.size(); i < length; ++i)
    {
        char ch = text[i];
        if (!std::isdigit(ch))
        {
            result = Error::CorruptedArchive;
            return;
        }
        if (intVal < (std::numeric_limits<uint64_t>::max() - 9) / 10)
        {
            intVal = 10 * intVal + (ch - '0');
        }
        else
        {
            if (intVal <= std::numeric_limits<uint64_t>::max() / 10)
            {
                intVal *= 10;
                if (intVal <= std::numeric_limits<uint64_t>::max() - (ch - '0'))
                {
                    intVal += ch - '0';
                }
                else
                {
                    result = Error::CorruptedArchive;
                    return;
                }
            }
            else
            {
                result = Error::CorruptedArchive;
                return;
            }
        }
    }

    val = intVal;
    result = Error::NoError;
}

template <>
void Deserializer::process<bool>(Error& result, bool& val)
{
    std::string text;
    m_in >> text;

    if (text == "true")
    {
        val = true;
    }
    else if (text == "false")
    {
        val = false;
    }
    else
    {
        result = Error::CorruptedArchive;
        return;
    }

    result = Error::NoError;
}
