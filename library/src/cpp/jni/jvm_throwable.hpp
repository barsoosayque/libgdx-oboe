#pragma once

#include "jni_context.hpp"
#include "jvm_class.hpp"

template<class T>
class jvm_throwable {
public:
    jvm_throwable(std::string_view msg) : m_cls(T::cls.data()), m_msg(msg) { }

    void throw_exception() {
        auto env = jni_context::acquire_thread();
        jstring msg = env->NewStringUTF(m_msg.data());
        jvm_object<jobject> exception(m_cls.construct(msg));
        env->Throw(exception.as<jthrowable>());
    }

private:
    jvm_class m_cls;
    std::string m_msg;
};