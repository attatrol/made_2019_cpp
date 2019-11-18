#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <ostream>

#include "error.h"

class Serializer
{
    static constexpr char Separator = ' ';
public:
    Serializer(std::ostream& out)
        : m_out(out)
    {
    }

    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(const ArgsT&... args)
    {
        Error result = Error::NoError;
        process(result, args...);
        return result;
    }

private:
    std::ostream& m_out;

    template <typename T>
    void process(Error& result, const T& val)
    {
        result = Error::UnsupportedType;
    }

    template <typename T, typename... Args>
    void process(Error& result, const T& val, const Args&... args)
    {
        process(result, std::forward<const T&>(val)); // process(result, std::forward<const T&>(val));
        if (result != Error::NoError)
        {
            return;
        }
        process(result, args...); // process(result, std::forward<const args&>(val));
    }
};

template <>
void Serializer::process<uint64_t>(Error& result, const uint64_t& val);

template <>
void Serializer::process<bool>(Error& result, const bool& val);

#endif // SERIALIZER_H
