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
    jvm_class(std::string_view class_name)
            : m_class_name(class_name)
            , m_class_name_hash(std::hash<std::string_view>()(class_name)) {
        auto it = g_class_cache.find(m_class_name_hash);
        if (it != g_class_cache.end()) {
            m_class = it->second;
        } else {
#ifdef REPORT_UNCACHED_JNI
            warn("jvm_class: usage of uncached jclass \"{}\"", class_name);
#endif
            auto context = jni_context::acquire_thread();
            m_class = jvm_object(context->FindClass(class_name.data()));
        }
    }

    jvm_class(jclass cls) : m_class(cls) {
        auto context = jni_context::acquire_thread();
        jclass cls_class = context->GetObjectClass(m_class);
        jmethodID get_name = context->GetMethodID(cls_class, "getName", "()Ljava/lang/String;");
        jstring name = static_cast<jstring>(context->CallObjectMethod(m_class, get_name));
        m_class_name = std::string{ context->GetStringUTFChars(name, NULL) };
        m_class_name_hash = std::hash<std::string>()(m_class_name);
        context->DeleteLocalRef(name);
    }

    template<class... Args>
    jobject construct(Args... args) {
        auto context = jni_context::acquire_thread();
        auto constructor = find_method<void(Args...)>("<init>");
        return context->NewObject(m_class, constructor, args...);
    }

    template<class F>
    jmethodID find_method(std::string_view name) {
        auto it = g_method_cache.find(gen_method_hash<F>(m_class_name, name));
        if (it != g_method_cache.end()) {
            return it->second;
        } else {
#ifdef REPORT_UNCACHED_JNI
            warn("jvm_class: usage of uncached jmethodID \"{}\" (class: \"{}\")", name, m_class_name);
#endif
            auto context = jni_context::acquire_thread();
            return context->GetMethodID(m_class, name.data(), jvm_signature<F>());
        }
    }

    template<class F>
    jfieldID find_field(std::string_view name) {
        auto it = g_field_cache.find(gen_field_hash(m_class_name, name));
        if (it != g_field_cache.end()) {
            return it->second;
        } else {
#ifdef REPORT_UNCACHED_JNI
            warn("jvm_class: usage of uncached jfieldID \"{}\" (class: \"{}\")", name, m_class_name);
#endif
            auto context = jni_context::acquire_thread();
            return context->GetFieldID(m_class, name.data(), jvm_signature<F>());
        }
    }

    template<class F>
    auto get_field(jobject obj, std::string_view name) {
        auto context = jni_context::acquire_thread();
        auto field = find_field<F>(name);

        if constexpr (std::is_same<F, int>::value)
            return context->GetIntField(obj, field);
        else if constexpr (std::is_same<F, bool>::value)
            return context->GetBooleanField(obj, field);
        else if constexpr (std::is_same<F, char>::value)
            return context->GetCharField(obj, field);
        else if constexpr (std::is_same<F, short>::value)
            return context->GetShortField(obj, field);
        else if constexpr (std::is_same<F, long>::value)
            return context->GetLongField(obj, field);
        else if constexpr (std::is_same<F, float>::value)
            return context->GetFloatField(obj, field);
        else if constexpr (std::is_same<F, double>::value)
            return context->GetDoubleField(obj, field);
        else
            return static_cast<F>(context->GetObjectField(obj, field));
    }

    template<class F, class... Args>
    auto execute_method(jobject obj, std::string_view name, Args... args) {
        using return_type = typename std::invoke_result<F, Args...>::type;
        auto context = jni_context::acquire_thread();
        auto method = find_method<F>(name);

        if constexpr (std::is_same<return_type, int>::value) {
            return context->CallIntMethod(obj, method, args...);
        } else if constexpr (std::is_same<return_type, void>::value) {
            return context->CallVoidMethod(obj, method, args...);
        } else if constexpr (std::is_same<return_type, bool>::value) {
            return context->CallBooleanMethod(obj, method, args...);
        } else if constexpr (std::is_same<return_type, char>::value) {
            return context->CallCharMethod(obj, method, args...);
        } else if constexpr (std::is_same<return_type, short>::value) {
            return context->CallShortMethod(obj, method, args...);
        } else if constexpr (std::is_same<return_type, long>::value) {
            return context->CallLongMethod(obj, method, args...);
        } else if constexpr (std::is_same<return_type, float>::value) {
            return context->CallFloatMethod(obj, method, args...);
        } else if constexpr (std::is_same<return_type, double>::value) {
            return context->CallDoubleMethod(obj, method, args...);
        } else {
            return static_cast<return_type>(context->CallObjectMethod(obj, method, args...));
        }
    }

    jclass const operator*() const { return m_class; }

    static void cache_class(std::string_view class_name) {
        auto context = jni_context::acquire_thread();
        g_class_cache[std::hash<std::string_view>()(class_name)] =
                jvm_object(context->FindClass(class_name.data()));
    }

    template<class F>
    static void cache_field(std::string_view class_name, std::string_view field_name) {
        auto context = jni_context::acquire_thread();
        std::size_t class_hash = std::hash<std::string_view>()(class_name);
        auto it = g_class_cache.find(class_hash);
        if (it != g_class_cache.end()) {
            g_field_cache[gen_field_hash(class_name, field_name)] =
                    context->GetFieldID(it->second, field_name.data(), jvm_signature<F>());
        } else {
            warn("jvm_class: caching field \"{}\" with uncached class \"{}\". Skipping...",
                 field_name, class_name);
        }
    }

    template<class F>
    static void cache_method(std::string_view class_name, std::string_view method_name) {
        auto context = jni_context::acquire_thread();
        std::size_t class_hash = std::hash<std::string_view>()(class_name);
        auto it = g_class_cache.find(class_hash);
        if (it != g_class_cache.end()) {
            g_method_cache[gen_method_hash<F>(class_name, method_name)] =
                    context->GetMethodID(it->second, method_name.data(), jvm_signature<F>());
        } else {
            warn("jvm_class: caching method \"{}\" with uncached class \"{}\". Skipping...",
                 method_name, class_name);
        }
    }

    using class_cache_map = std::unordered_map<std::size_t, jvm_object<jclass>>;
    using field_cache_map = std::unordered_map<std::size_t, jfieldID>;
    using method_cache_map = std::unordered_map<std::size_t, jmethodID>;
private:
    static class_cache_map g_class_cache;
    static field_cache_map g_field_cache;
    static method_cache_map g_method_cache;

    inline static std::size_t gen_field_hash(std::string_view class_name,
                                             std::string_view field_name) {
        return merge_hash(class_name, field_name);
    }

    template<class F>
    inline static std::size_t gen_method_hash(std::string_view class_name,
                                              std::string_view method_name) {
        return merge_hash(class_name, method_name, jvm_signature<F>());
    }

    jvm_object<jclass> m_class;
    std::string m_class_name;
    std::size_t m_class_name_hash;
};