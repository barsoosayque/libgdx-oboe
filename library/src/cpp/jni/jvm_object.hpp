#pragma once
#include <functional>
#include <memory>
#include "jni_context.hpp"
#include <jni.h>

template <class T = jobject>
class jvm_object {
    public:
        // wrap some object
        jvm_object(jni_context p_context, T p_obj)
            : m_wrapper(std::make_shared<wrapper>(p_context, p_obj)) {}

        // with custom function which called when object is being
        // garbage-collected
        template <class D>
        jvm_object(jni_context p_context, T p_obj, D p_on_delete)
            : m_wrapper(std::make_shared<wrapper>(p_context, p_obj)) {
            m_wrapper->m_on_delete = p_on_delete;
        }

        operator T() const { return m_wrapper->m_obj; }
        T operator->() const { return m_wrapper->m_obj; }

        template <class N>
        N as() const { return static_cast<N>(m_wrapper->m_obj); }
    private:
        class wrapper {
            public:
                wrapper(jni_context p_context, T p_obj)
                    : m_context(p_context)
                    , m_obj(static_cast<T>(p_context->NewGlobalRef(p_obj))) {}

                ~wrapper() {
                    if(m_on_delete) {
                        m_on_delete(m_obj);
                    }
                    m_context->DeleteGlobalRef(m_obj);
                }

                std::function<void(T&)> m_on_delete;
                jni_context m_context;
                T m_obj;
        };

        std::shared_ptr<wrapper> m_wrapper;
};