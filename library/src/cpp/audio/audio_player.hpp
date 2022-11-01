#pragma once

#include "renderable_audio.hpp"
#include "oboe_engine.hpp"
#include <memory>
#include <vector>
#include <atomic>

/// oboe_engine frontend for playing renderable_audio.
class audio_player {
public:
    /// Creates a default audio_player.
    audio_player();

    /// Plays a renderable_audio implementation.
    void play_audio(const std::shared_ptr<renderable_audio> &audio);

    /// Set volume of audio_player to value.
    void volume(float volume) { m_volume = std::clamp(volume, 0.0f, 1.0f); };
    /// Get volume of audio_player.
    float volume() const { return m_volume; }

    /// Stop any audio.
    void stop() { m_engine.stop(); }
    /// Resume playing audio.
    void resume() { m_engine.resume(); }
private:
    const std::vector<int16_t>& generate_audio(uint32_t num_frames);

    oboe_engine m_engine;
    float m_volume;

    std::vector<int16_t> m_pcm;
    std::vector<int16_t> m_buffer;
    std::vector<std::weak_ptr<renderable_audio>> m_tracks;

    std::atomic_flag m_rendering_flag;
};