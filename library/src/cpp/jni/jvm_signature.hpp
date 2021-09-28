#pragma once

#include "jni.h"
#include <frozen/string.hpp>
#include <string_view>

using frozen::operator "" _fstr;

// --- Definitions ---
template<class T>
struct jvm_signature_t;
template<class... Targs>
struct jvm_params_signature_t;

// --- User API ---
template<class T>
constexpr char const *jvm_signature() { return jvm_signature_t<T>::value.data(); }

// --- Simple types ---
template<>
struct jvm_signature_t<void> { static constexpr auto value = "V"_fstr; };
template<>
struct jvm_signature_t<bool> { static constexpr auto value = "Z"_fstr; };
template<>
struct jvm_signature_t<short> { static constexpr auto value = "S"_fstr; };
template<>
struct jvm_signature_t<int> { static constexpr auto value = "I"_fstr; };
template<>
struct jvm_signature_t<long> { static constexpr auto value = "J"_fstr; };
template<>
struct jvm_signature_t<float> { static constexpr auto value = "F"_fstr; };
template<>
struct jvm_signature_t<double> { static constexpr auto value = "D"_fstr; };
template<class T>
struct jvm_signature_t<T[]> { static constexpr auto value = "["_fstr + jvm_signature_t<T>::value; };

// --- JNI types ---
template<>
struct jvm_signature_t<jstring> { static constexpr auto value = "Ljava/lang/String;"_fstr; };
template<>
struct jvm_signature_t<jbyte> { static constexpr auto value = "B"_fstr; };
template<>
struct jvm_signature_t<jbyteArray> {
    static constexpr auto value = jvm_signature_t<jbyte[]>::value;
};

// --- Method type ---
template<>
struct jvm_params_signature_t<> { static constexpr auto value = ""_fstr; };
template<class T>
struct jvm_params_signature_t<T> { static constexpr auto value = jvm_signature_t<T>::value; };
template<class T, class ... Targs>
struct jvm_params_signature_t<T, Targs...> {
    static constexpr auto value = jvm_signature_t<T>::value +
                                  jvm_params_signature_t<Targs...>::value;
};
template<class R, class ... Args>
struct jvm_signature_t<R(Args...)> {
    static constexpr auto value = "("_fstr +
                                  jvm_params_signature_t<Args...>::value +
                                  ")"_fstr +
                                  jvm_signature_t<R>::value;
};

// --- Custom types ---

template<class Custom>
struct jvm_signature_t {
    static constexpr auto value = "[L"_fstr +
                                  Custom::cls +
                                  ";"_fstr;
};