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
        return process(args...);
    }

private:
    std::ostream& m_out;

    template <typename T>
    Error process(const T& val)
    {
        return Error::UnsupportedType;
    }
    Error process(const uint64_t& val);

    Error process(const bool& val);

    template <typename T, typename... Args>
    Error process(const T& val, const Args&... args)
    {
        Error result = process(std::forward<const T&>(val)); // process(result, std::forward<const T&>(val));
        if (result != Error::NoError)
        {
            return result;
        }
        return process(args...); // process(result, std::forward<const args&>(val));
    }
};

#endif // SERIALIZER_H
