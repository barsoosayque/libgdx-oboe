#pragma once
#include "jni.h"
#include "jvm_signature.hpp"
#include "jni_context.hpp"
#include "jvm_object.hpp"
#include "../utility/log.hpp"
#include "../utility/hash.hpp"
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
            jclass cls_class = context->GetObjectClass(m_class);
            jmethodID get_name = context->GetMethodID(cls_class, "getName", "()Ljava/lang/String;");
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
            auto it = g_method_cache.find(gen_method_hash<F>(m_class_name, p_name));
            if(it != g_method_cache.end()) {
                return it->second;
            } else {
#ifdef REPORT_UNCACHED_JNI
                warn("jvm_class: usage of uncached jmethodID \"{}\" (class: \"{}\")", p_name, m_class_name);
#endif
                auto context = jni_context::acquire_thread();
                return context->GetMethodID(m_class, p_name.data(), jvm_signature<F>());
            }
        }

        template <class F>
        jfieldID find_field(std::string_view p_name) {
            auto it = g_field_cache.find(gen_field_hash(m_class_name, p_name));
            if(it != g_field_cache.end()) {
                return it->second;
            } else {
#ifdef REPORT_UNCACHED_JNI
                warn("jvm_class: usage of uncached jfieldID \"{}\" (class: \"{}\")", p_name, m_class_name);
#endif
                auto context = jni_context::acquire_thread();
                return context->GetFieldID(m_class, p_name.data(), jvm_signature<F>());
            }
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

        static void cache_class(std::string_view p_class_name) {
            auto context = jni_context::acquire_thread();
            g_class_cache[std::hash<std::string_view>()(p_class_name)] =
                jvm_object(context->FindClass(p_class_name.data()));
        }
        template <class F>
        static void cache_field(std::string_view p_class_name, std::string_view p_field_name) {
            auto context = jni_context::acquire_thread();
            std::size_t class_hash = std::hash<std::string_view>()(p_class_name);
            auto it = g_class_cache.find(class_hash);
            if(it != g_class_cache.end()) {
                g_field_cache[gen_field_hash(p_class_name, p_field_name)] =
                    context->GetFieldID(it->second, p_field_name.data(), jvm_signature<F>());
            } else {
                warn("jvm_class: caching field \"{}\" with uncached class \"{}\". Skipping...", p_field_name, p_class_name);
            }
        }
        template <class F>
        static void cache_method(std::string_view p_class_name, std::string_view p_method_name) {
            auto context = jni_context::acquire_thread();
            std::size_t class_hash = std::hash<std::string_view>()(p_class_name);
            auto it = g_class_cache.find(class_hash);
            if(it != g_class_cache.end()) {
                g_method_cache[gen_method_hash<F>(p_class_name, p_method_name)] =
                    context->GetMethodID(it->second, p_method_name.data(), jvm_signature<F>());
            } else {
                warn("jvm_class: caching method \"{}\" with uncached class \"{}\". Skipping...", p_method_name, p_class_name);
            }
        }

        using class_cache_map = std::unordered_map<std::size_t, jvm_object<jclass>>;
        using field_cache_map = std::unordered_map<std::size_t, jfieldID>;
        using method_cache_map = std::unordered_map<std::size_t, jmethodID>;
    private:
        static class_cache_map g_class_cache;
        static field_cache_map g_field_cache;
        static method_cache_map g_method_cache;

        inline static std::size_t gen_field_hash(std::string_view p_class_name, std::string_view p_field_name) {
            return merge_hash(p_class_name, p_field_name);
        }

        template <class F>
        inline static std::size_t gen_method_hash(std::string_view p_class_name, std::string_view p_method_name) {
            return merge_hash(p_class_name, p_method_name, jvm_signature<F>());
        }

        jvm_object<jclass> m_class;
        std::string m_class_name;
        std::size_t m_class_name_hash;
};