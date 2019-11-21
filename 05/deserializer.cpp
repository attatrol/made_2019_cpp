#include <limits>

#include "deserializer.h"

Error Deserializer::process(uint64_t& val)
{
    std::string text;
    m_in >> text;

    uint64_t intVal = 0;

    for (std::size_t i = 0, length = text.size(); i < length; ++i)
    {
        char ch = text[i];
        if (!std::isdigit(ch))
        {
            return Error::CorruptedArchive;
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
                    return Error::CorruptedArchive;
                }
            }
            else
            {
                return Error::CorruptedArchive;
            }
        }
    }

    val = intVal;
    return Error::NoError;
}

Error Deserializer::process(bool& val)
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
        return Error::CorruptedArchive;
    }

    return Error::NoError;
}
