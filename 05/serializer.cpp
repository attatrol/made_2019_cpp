#include "serializer.h"

Error Serializer::process(const uint64_t& val)
{
    m_out << val << Separator;
    return Error::NoError;
}

Error Serializer::process(const bool& val)
{
    m_out << (val ? "true" : "false") << Separator;
    return Error::NoError;
}
