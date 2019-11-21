#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <istream>

#include "error.h"

class Deserializer
{
    static constexpr char Separator = ' ';
public:
    Deserializer(std::istream& in_)
        : m_in(in_)
    {
    }

    template <class T>
    Error load(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&... args)
    {
        return process(args...);
    }

private:
    std::istream& m_in;

    template <typename T>
    Error process(T& val)
    {
        return Error::UnsupportedType;
    }
    Error process(uint64_t& val);
    Error process(bool& val);

    template <typename T, typename... Args>
    Error process(T& val, Args&... args)
    {
        Error result = process(val);
        if (result != Error::NoError)
        {
            return result;
        }
        return process(args...);
    }
};

#endif // DESERIALIZER_H
