#pragma once

#include "oboe_engine.hpp"

/// oboe_engine frontend for reading PCM (blocking).
class audio_recorder {
public:
    audio_recorder(uint8_t channels, uint32_t sample_rate)
        : m_engine(oboe_engine::mode::reading, channels, sample_rate) {
        m_engine.resume();
    }

    template<class iterator>
    void read(iterator begin, iterator end) {
        auto len = std::distance(begin, end);
        m_engine.blocking_read(begin, len);
    }

private:
    oboe_engine m_engine;
};