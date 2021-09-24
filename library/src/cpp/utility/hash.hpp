#pragma once

inline void hash_combine(std::size_t &seed) { }

template<typename T, typename... Rest>
inline void hash_combine(std::size_t &seed, const T &v, Rest... rest) {
    std::hash<T> hash;
    seed ^= hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

template<typename T, typename... Rest>
inline std::size_t merge_hash(const T &v, Rest... rest) {
    std::size_t seed = 0;
    hash_combine(seed, v, rest...);
    return seed;
}
