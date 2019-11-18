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
        Error result = Error::NoError;
        process(result, args...);
        return result;
    }

private:
    std::istream& m_in;

    template <typename T>
    void process(Error& result, T& val)
    {
        result = Error::UnsupportedType;
    }

    template <typename T, typename... Args>
    void process(Error& result, T& val, Args&... args)
    {
        process(result, val);
        if (result != Error::NoError)
        {
            return;
        }
        process(result, args...);
    }
};

template <>
void Deserializer::process<uint64_t>(Error& result, uint64_t& val);

template <>
void Deserializer::process<bool>(Error& result, bool& val);

#endif // DESERIALIZER_H
