#pragma once

#include "../jni/jvm_signature.hpp"
#include "../jni/jvm_throwable.hpp"
#include "../native/external_classes.hpp"

inline void throw_exception(std::string_view msg) {
    jvm_throwable<external::gdx_runtime_exception>(msg).throw_exception();
}

template<class ... Params>
inline void throw_exception(std::string_view format, Params... params) {
    jvm_throwable<external::gdx_runtime_exception>(fmt::format(format, params...)).throw_exception();
}