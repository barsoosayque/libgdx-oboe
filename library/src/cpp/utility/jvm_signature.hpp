#pragma once
#include <frozen/string.hpp>
#include <string_view>

// --- Definitions ---
template <class T>
struct jvm_signature_t;
template <class... Targs>
struct jvm_params_signature_t;

// --- User API ---
template <class T>
constexpr char const * jvm_signature() { return jvm_signature_t<T>::value.data(); }

// --- Simple types ---
template <>
struct jvm_signature_t<void> { static constexpr auto value = frozen::make_string("V"); };
template <>
struct jvm_signature_t<bool> { static constexpr auto value = frozen::make_string("Z"); };
template <>
struct jvm_signature_t<short> { static constexpr auto value = frozen::make_string("S"); };
template <>
struct jvm_signature_t<int> { static constexpr auto value = frozen::make_string("I"); };
template <>
struct jvm_signature_t<long> { static constexpr auto value = frozen::make_string("J"); };
template <>
struct jvm_signature_t<float> { static constexpr auto value = frozen::make_string("F"); };
template <>
struct jvm_signature_t<double> { static constexpr auto value = frozen::make_string("D"); };
template <class T>
struct jvm_signature_t<T[]> { static constexpr auto value = frozen::make_string("[") + jvm_signature_t<T>::value; };

// --- Method type ---
template <class T>
struct jvm_params_signature_t<T> { static constexpr auto value = jvm_signature_t<T>::value; };
template <class T, class ... Targs>
struct jvm_params_signature_t<T, Targs...> { static constexpr auto value = jvm_signature_t<T>::value +
                                                                           jvm_params_signature_t<Targs...>::value; };
template <class R, class ... Args>
struct jvm_signature_t<R(Args...)> { static constexpr auto value = frozen::make_string("(") +
                                                                   jvm_params_signature_t<Args...>::value +
                                                                   frozen::make_string(")") +
                                                                   jvm_signature_t<R>::value; };

// --- Some custom types ---

struct String { static constexpr auto cls = frozen::make_string("java/lang/String"); };

template <class Custom>
struct jvm_signature_t { static constexpr auto value = frozen::make_string("L") +
                                                       Custom::cls +
                                                       frozen::make_string(";"); };
