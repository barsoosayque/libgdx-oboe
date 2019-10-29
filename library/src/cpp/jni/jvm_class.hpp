#pragma once
#include "jni.h"
#include "jvm_signature.hpp"
#include "jni_context.hpp"
#include "jvm_object.hpp"
#include "../utility/log.hpp"
#include <memory>
#include <string_view>
#include <type_traits>
#include <unordered_map>

class jvm_class {
    public:
        jvm_class(std::string_view p_class_name)
            : m_class_name(p_class_name)
            , m_class_name_hash(std::hash<std::string_view>()(p_class_name))
        {
            auto it = g_class_cache.find(m_class_name_hash);
            if(it != g_class_cache.end()) {
                m_class = it->second;
            } else {
#ifdef REPORT_UNCACHED_JNI
                warn("jvm_class: usage of uncached jclass \"{}\"", p_class_name);
#endif
                auto context = jni_context::acquire_thread();
                m_class = jvm_object(context->FindClass(p_class_name.data()));
            }
        }

        jvm_class(jclass p_class) : m_class(p_class) {
            auto context = jni_context::acquire_thread();
            jmethodID get_name = context->GetMethodID(m_class, "getName", "()Ljava/lang/String;");
            jstring name = static_cast<jstring>(context->CallObjectMethod(m_class, get_name));
            m_class_name = std::string{context->GetStringUTFChars(name, NULL)};
            m_class_name_hash = std::hash<std::string>()(m_class_name);
            context->DeleteLocalRef(name);
        }

        template <class... Args>
        jobject construct(Args... p_args) {
            auto context = jni_context::acquire_thread();
            auto constructor = find_method<void(Args...)>("<init>");
            return context->NewObject(m_class, constructor, p_args...);
        }

        template <class F>
        jmethodID find_method(std::string_view p_name) {
            auto method_map = g_method_cache.find(m_class_name_hash);
            if(method_map != g_method_cache.end()) {
                auto it = method_map->second.find(std::hash<std::string_view>()(p_name));
                if(it != method_map->second.end()) {
                    return it->second;
                }
            }
#ifdef REPORT_UNCACHED_JNI
            warn("jvm_class: usage of uncached jmethodID \"{}\" (class: \"{}\")", p_name, m_class_name);
#endif
            auto context = jni_context::acquire_thread();
            return context->GetMethodID(m_class, p_name.data(), jvm_signature<F>());
        }

        template <class F>
        jfieldID find_field(std::string_view p_name) {
            auto field_map = g_field_cache.find(m_class_name_hash);
            if(field_map != g_field_cache.end()) {
                auto it = field_map->second.find(std::hash<std::string_view>()(p_name));
                if(it != field_map->second.end()) {
                    return it->second;
                }
            }
#ifdef REPORT_UNCACHED_JNI
            warn("jvm_class: usage of uncached jfieldID \"{}\" (class: \"{}\")", p_name, m_class_name);
#endif
            auto context = jni_context::acquire_thread();
            return context->GetFieldID(m_class, p_name.data(), jvm_signature<F>());
        }

        template <class F>
        auto get_field(jobject p_obj, std::string_view p_name) {
            auto context = jni_context::acquire_thread();
            auto field = find_field<F>(p_name);

            if constexpr (std::is_same<F, int>::value) return context->GetIntField(p_obj, field);
            else if constexpr (std::is_same<F, bool>::value) return context->GetBooleanField(p_obj, field);
            else if constexpr (std::is_same<F, char>::value) return context->GetCharField(p_obj, field);
            else if constexpr (std::is_same<F, short>::value) return context->GetShortField(p_obj, field);
            else if constexpr (std::is_same<F, long>::value) return context->GetLongField(p_obj, field);
            else if constexpr (std::is_same<F, float>::value) return context->GetFloatField(p_obj, field);
            else if constexpr (std::is_same<F, double>::value) return context->GetDoubleField(p_obj, field);
            else return static_cast<F>(context->GetObjectField(p_obj, field));
        }

        template <class F, class... Args>
        auto execute_method(jobject p_obj, std::string_view p_name, Args... p_args) {
            using return_type = typename std::invoke_result<F, Args...>::type;
            auto context = jni_context::acquire_thread();
            auto method = find_method<F>(p_name);

            if constexpr (std::is_same<return_type, int>::value) {
                return context->CallIntMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, void>::value) {
                return context->CallVoidMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, bool>::value) {
                return context->CallBooleanMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, char>::value) {
                return context->CallCharMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, short>::value) {
                return context->CallShortMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, long>::value) {
                return context->CallLongMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, float>::value) {
                return context->CallFloatMethod(p_obj, method, p_args...);
            }
            else if constexpr (std::is_same<return_type, double>::value) {
                return context->CallDoubleMethod(p_obj, method, p_args...);
            }
            else {
                return static_cast<return_type>(context->CallObjectMethod(p_obj, method, p_args...));
            }
        }

        static jclass cache_class(std::string_view p_class_name) {
            return 0;
        }
        static jfieldID cache_field(std::string_view p_class_name, std::string_view p_field_name) {
            return 0;
        }
        static jmethodID cache_method(std::string_view p_class_name, std::string_view p_method_name) {
            return 0;
        }
    private:
        static std::unordered_map<std::size_t, jclass> g_class_cache;
        static std::unordered_map<std::size_t, std::unordered_map<std::size_t, jfieldID>> g_field_cache;
        static std::unordered_map<std::size_t, std::unordered_map<std::size_t, jmethodID>> g_method_cache;

        jvm_object<jclass> m_class;
        std::string m_class_name;
        std::size_t m_class_name_hash;
};