#pragma once

#include <variant>
#include <type_traits>

/// Success-value wrapper for result class
template<class T>
struct ok {
    ok(T &val) : m_value(val) {}
    ok(T &&val) : m_value(std::move(val)) {}

    T m_value;
};

/// Error-value wrapper for result class
template<class E>
struct err {
    err(E &val) : m_value(val) {}
    err(E &&val) : m_value(std::move(val)) {}

    E m_value;
};

/// Rust-like implementation of result class
/// @see https://doc.rust-lang.org/std/result/enum.Result.html
template<class T, class E>
class result {
public:
    using ok_t = ok<T>;
    using ok_value_t = T;
    using err_t = err<E>;
    using err_value_t = E;

    result(ok_t &&success) : m_storage(std::move(success)) {}
    result(err_t &&error) : m_storage(std::move(error)) {}
    result(result &&other) : m_storage(std::move(other.m_storage)) {}

    /// Check if result is a success
    bool is_ok() const { return std::holds_alternative<ok_t>(m_storage); }

    /// Check if result is an error
    bool is_err() const { return std::holds_alternative<err_t>(m_storage); }

    /// Returns the contained ok value by move
    T &&unwrap() { return std::move(std::get<ok_t>(m_storage).m_value); }

    /// Returns the contained err value by move
    E &&unwrap_err() { return std::move(std::get<err_t>(m_storage).m_value); }

    /// Maps a result<T, E> to result<U, E> by applying a function
    /// to a contained ok value, leaving an err value untouched
    template<class Func, class U = typename std::invoke_result_t<Func, T &&>>
    inline result<U, E> map(Func f) {
        return is_ok()? result<U, E>(ok<U>(f(unwrap()))) : result<U, E>(unwrap_err());
    }

    /// Returns a new result if self is ok, otherwise returns the err value of self
    template<class Func, class U = typename std::invoke_result_t<Func, T &&>::ok_value_t>
    inline result<U, E> and_then(Func f) {
        static_assert(
                std::is_same_v<typename std::invoke_result_t<Func, T &&>::err_value_t, E>,
                "and_then error type must be the same");

        return is_ok()? f(unwrap()) : result<U, E>(unwrap_err());
    }

    /// Returns the contained ok value by move or computes it from a function
    template<class Func>
    inline T unwrap_or_else(Func f) {
        static_assert(
                std::is_constructible_v<T, std::invoke_result_t<Func, E &&>>,
                "unwrap_or_else return must be either an ok type or be convertible to ok type");

        return is_ok()? unwrap() : T(f(unwrap_err()));
    }


private:
    std::variant<ok_t, err_t> m_storage;
};

// shamelessly stolen from oktal/result, but adapted 😎
#define TRY(...)                                                   \
    ({                                                             \
        auto res = __VA_ARGS__;                                    \
        if (res.is_err()) {                                        \
            return err(res.unwrap_err());                          \
        }                                                          \
        res.unwrap();                                              \
    })