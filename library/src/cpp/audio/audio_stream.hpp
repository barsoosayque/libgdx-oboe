#pragma once

#include "oboe_engine.hpp"

/// oboe_engine frontend for writing PCM directly (blocking).
class audio_stream {
public:
    /// Creates an audio_stream with set channel count and sample rate.
    audio_stream(uint8_t channels, uint32_t sample_rate);

    /// Write 16bit PCM into the stream.
    void write(std::vector<int16_t> &&pcm);
    /// Write Float Pcm into the stream.
    void write(std::vector<float> &&pcm);

    /// Set volume of audio_stream to value.
    void volume(float volume) { m_volume = std::clamp(volume, 0.0f, 1.0f); };
    /// Get volume of audio_stream.
    float volume() const { return m_volume; }

    /// Stop any audio.
    void stop() { m_engine.stop(); }
    /// Resume playing audio.
    void resume() { m_engine.resume(); }

    /// Get latency of underlying oboe stream.
    uint32_t latency() const { return m_engine.payload_size(); }
    /// Return true if this stream is mono.
    bool is_mono() const { return m_engine.is_mono(); }
private:
    oboe_engine m_engine;
    float m_volume;

    std::vector<int16_t> m_pcm_buffer;
};