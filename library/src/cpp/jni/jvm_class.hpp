#pragma once
#include "jni.h"
#include "jvm_signature.hpp"
#include "jni_context.hpp"
#include "jvm_object.hpp"
#include <memory>
#include <string_view>
#include <type_traits>

// TODO: cache fieldID and methodID
class jvm_class {
    public:
        jvm_class(jni_context& p_context, std::string_view p_class_name)
            : m_context(p_context)
            , m_class(m_context, p_context->FindClass(p_class_name.data())) {}

        jvm_class(jni_context& p_context, jclass p_class)
            : m_context(p_context)
            , m_class(m_context, p_class) {}

        template <class... Args>
        jobject construct(Args... p_args) {
            auto constructor = find_method<void(Args...)>("<init>");
            return m_context->NewObject(m_class, constructor, p_args...);
        }

        template <class F>
        jmethodID find_method(std::string_view p_name) {
            return m_context->GetMethodID(m_class, p_name.data(), jvm_signature<F>());
        }

        template <class F>
        jfieldID find_field(std::string_view p_name) {
            return m_context->GetFieldID(m_class, p_name.data(), jvm_signature<F>());
        }

        template <class F>
        auto get_field(jobject p_obj, std::string_view p_name) {
            auto field = find_field<F>(p_name);

            if constexpr (std::is_same<F, int>::value) return m_context->GetIntField(p_obj, field);
            else if constexpr (std::is_same<F, bool>::value) return m_context->GetBooleanField(p_obj, field);
            else if constexpr (std::is_same<F, char>::value) return m_context->GetCharField(p_obj, field);
            else if constexpr (std::is_same<F, short>::value) return m_context->GetShortField(p_obj, field);
            else if constexpr (std::is_same<F, long>::value) return m_context->GetLongField(p_obj, field);
            else if constexpr (std::is_same<F, float>::value) return m_context->GetFloatField(p_obj, field);
            else if constexpr (std::is_same<F, double>::value) return m_context->GetDoubleField(p_obj, field);
            else return static_cast<F>(m_context->GetObjectField(p_obj, field));
        }

        template <class F, class... Args>
        auto execute_method(jobject p_obj, std::string_view p_name, Args... p_args) {
            using return_type = typename std::invoke_result<F, Args...>::type;
            auto method = find_method<F>(p_name);

            if constexpr (std::is_same<return_type, int>::value) {
                return m_context->CallIntMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, void>::value) {
                return m_context->CallVoidMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, bool>::value) {
                return m_context->CallBooleanMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, char>::value) {
                return m_context->CallCharMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, short>::value) {
                return m_context->CallShortMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, long>::value) {
                return m_context->CallLongMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, float>::value) {
                return m_context->CallFloatMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, double>::value) {
                return m_context->CallDoubleMethod(p_obj, method, p_args...);
            }
            else {
                return static_cast<return_type>(m_context->CallObjectMethod(p_obj, method, p_args...));
            }
        }
    private:
        jni_context m_context;
        jvm_object<jclass> m_class;
};