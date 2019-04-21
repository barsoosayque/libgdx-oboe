#pragma once
#include <jni.h>

template <class T>
struct jni_array_type_t;

template <>
struct jni_array_type_t<jshortArray> { using value = int16_t; };
template <>
struct jni_array_type_t<jfloatArray> { using value = float; };
// probably do more of this, but I don't need them

template <class A>
class jni_array {
    public:
        using type = jni_array_type_t<A>;

        jni_array(jni_context p_context, A p_array)
            : m_context(p_context)
            , m_array(p_array)
            , m_size(p_context->GetArrayLength(p_array)) {
            if constexpr (std::is_same<type, int16_t>::value) {
                m_data = p_context->GetShortArrayElements(p_array, 0);
            } else if constexpr (std::is_same<type, float>::value) {
                m_data = p_context->GetFloatArrayElements(p_array, 0);
            }
        }

        ~jni_array() {
            if constexpr (std::is_same<type, int16_t>::value) {
                m_context->ReleaseShortArrayElements(m_array, m_data, 0);
            } else if constexpr (std::is_same<type, float>::value) {
                m_context->ReleaseFloatArrayElements(m_array, m_data, 0);
            }
        }

        int size() const { return m_size; }
        type* data() const { return m_data; }
    private:
        A m_array;
        jni_context m_context;
        const type* m_data;
        jsize m_size;
};
