#pragma once
#include "jni.h"
#include "../utility/jvm_signature.hpp"
#include <string_view>
#include <type_traits>

class jvm_class {
    public:
        jvm_class(JNIEnv* p_env, std::string_view p_class_name)
            : m_class(p_env->FindClass(p_class_name.data())) {
            p_env->GetJavaVM(&m_jvm);
        }

        template <class... Args>
        jobject construct(Args... p_args) {
            auto constructor = find_method<void(Args...)>("<init>");
            return env()->NewObject(m_class, constructor, p_args...);
        }

        template <class F>
        jmethodID find_method(std::string_view p_name) {
            return env()->GetMethodID(m_class, p_name.data(), jvm_signature<F>());
        }

        template <class F, class... Args>
        auto execute_method(jobject p_obj, std::string_view p_name, Args... p_args) {
            using return_type = typename std::invoke_result<F, Args...>::type;
            auto method = find_method<F>(p_name);

            if constexpr (std::is_same<return_type, int>::value) {
                return env()->CallIntMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, void>::value) {
                return env()->CallVoidMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, bool>::value) {
                return env()->CallBooleanMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, char>::value) {
                return env()->CallCharMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, short>::value) {
                return env()->CallShortMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, long>::value) {
                return env()->CallLongMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, float>::value) {
                return env()->CallFloatMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, double>::value) {
                return env()->CallDoubleMethod(p_obj, method, p_args...);
            }
            else { return env()->CallObjectMethod(p_obj, method, p_args...); }
        }
    private:
        inline JNIEnv* env() {
            JNIEnv* env;
            m_jvm->AttachCurrentThread(&env, NULL);
            return env;
        }

        JavaVM* m_jvm;
        jclass m_class;
};