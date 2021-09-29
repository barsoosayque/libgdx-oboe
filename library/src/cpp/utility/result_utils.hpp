#pragma once

#include "result.hpp"
#include <fmt/format.h>

struct simple_error { std::string m_text; };

template<class ... Params>
auto make_error(std::string_view format, Params... params) {
    return err(simple_error{ fmt::format(format, params...) });
}

template<class E>
auto make_error(std::string_view msg) {
    return err(simple_error{ std::string(msg) });
}

#define SIMPLE_RESULT(name) \
class name; \
using name ## _result = result<name, simple_error>;
