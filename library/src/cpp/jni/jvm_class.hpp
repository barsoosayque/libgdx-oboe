#pragma once
#include "jni.h"
#include "../utility/jvm_signature.hpp"
#include "jni_context.hpp"
#include "jvm_object.hpp"
#include <memory>
#include <string_view>
#include <type_traits>

class jvm_class {
    public:
        jvm_class(jni_context p_context, std::string_view p_class_name)
            : m_context(p_context)
            , m_class(m_context, p_context.env()->FindClass(p_class_name.data())) {}

        template <class... Args>
        jobject construct(Args... p_args) {
            auto constructor = find_method<void(Args...)>("<init>");
            return m_context.env()->NewObject(m_class, constructor, p_args...);
        }

        template <class F>
        jmethodID find_method(std::string_view p_name) {
            return m_context.env()->GetMethodID(m_class, p_name.data(), jvm_signature<F>());
        }

        template <class F, class... Args>
        auto execute_method(jobject p_obj, std::string_view p_name, Args... p_args) {
            using return_type = typename std::invoke_result<F, Args...>::type;
            auto method = find_method<F>(p_name);

            if constexpr (std::is_same<return_type, int>::value) {
                return m_context.env()->CallIntMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, void>::value) {
                return m_context.env()->CallVoidMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, bool>::value) {
                return m_context.env()->CallBooleanMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, char>::value) {
                return m_context.env()->CallCharMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, short>::value) {
                return m_context.env()->CallShortMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, long>::value) {
                return m_context.env()->CallLongMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, float>::value) {
                return m_context.env()->CallFloatMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, double>::value) {
                return m_context.env()->CallDoubleMethod(p_obj, method, p_args...);
            }
            else {
                return static_cast<return_type>(m_context.env()->CallObjectMethod(p_obj, method, p_args...));
            }
        }
    private:
        jni_context m_context;
        jvm_object<jclass> m_class;
};