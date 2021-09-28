#pragma once

#include "jni_context.hpp"
#include "jvm_class.hpp"

template<class T>
class jvm_throwable {
public:
    jvm_throwable(std::string_view msg) : m_cls(jvm_signature<T>()), m_msg(msg) { }

    void throw_exception() {
        auto env = jni_context::acquire_thread();
        env->ThrowNew(*m_cls, m_msg.data());
    }

private:
    jvm_class m_cls;
    std::string m_msg;
};