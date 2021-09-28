#pragma once

#include "../jni/jvm_signature.hpp"
#include "../jni/jvm_throwable.hpp"

namespace {
struct GdxRuntimeException {
    static constexpr auto cls = "com/badlogic/gdx/utils/GdxRuntimeException"_fstr;
    jobject obj;
};
}

inline void throw_exception(std::string_view msg) {
    jvm_throwable<GdxRuntimeException>(msg).throw_exception();
}

template<class ... Params>
inline void throw_exception(std::string_view format, Params... params) {
    jvm_throwable<GdxRuntimeException>(fmt::format(format, params...)).throw_exception();
}