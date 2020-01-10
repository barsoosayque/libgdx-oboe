#pragma once
#include <memory>
#include "jni_context.hpp"
#include <jni.h>

template <class T = jobject>
class jvm_object {
    public:
        jvm_object() : m_wrapper(nullptr) {}

        // wrap some object
        jvm_object(T obj) : m_wrapper(std::make_shared<wrapper>(obj)) { }

        operator T() const { return m_wrapper->m_obj; }
        T operator->() const { return m_wrapper->m_obj; }

        template <class N>
        N as() const { return static_cast<N>(m_wrapper->m_obj); }
    private:
        class wrapper {
            public:
                wrapper(T obj) {
                        auto context = jni_context::acquire_thread();
                        m_obj = static_cast<T>(context->NewGlobalRef(obj));
                    }

                ~wrapper() {
                    auto context = jni_context::acquire_thread();
                    context->DeleteGlobalRef(m_obj);
                }

                T m_obj;
        };

        std::shared_ptr<wrapper> m_wrapper;
};