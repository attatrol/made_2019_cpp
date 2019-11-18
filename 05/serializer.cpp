#include "serializer.h"

template <>
void Serializer::process<uint64_t>(Error& result, const uint64_t& val)
{
    m_out << val << Separator;
}

template <>
void Serializer::process<bool>(Error& result, const bool& val)
{
    m_out << (val ? "true" : "false") << Separator;
}
