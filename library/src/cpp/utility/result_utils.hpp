#pragma once

#include <result.h>
#include <fmt/format.h>

template <class E, class ... Params>
auto make_error(std::string_view format, Params... params)
{
    return Err(E{fmt::format(format, params...)});
}

template <class E>
auto make_error(std::string_view msg)
{
    return Err(E{std::string(msg)});
}

#define SIMPLE_RESULT(name) \
struct name ## _error { std::string m_text; }; \
class name; \
using name ## _result = Result<name, name ## _error>;
