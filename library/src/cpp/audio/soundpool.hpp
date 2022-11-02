#pragma once

#include <vector>
#include "pan_effect.hpp"
#include "renderable_audio.hpp"
#include "../samplerate/resampler.hpp"
#include <functional>
#include <atomic>

/// Soundpool native implementation.
/// Uses fully loaded 16Bit PCM to play infinite number of sounds.
class soundpool : public renderable_audio {
public:
    using data_t = std::vector<int16_t>;

    /// Create a new soundpool with provided pcm data and set number of channels.
    soundpool(const data_t &pcm, int8_t channels);

    /// Play an new instance of sound with settings.
    /// @return id on new sound
    long play(float volume = 1.0f,
              float speed = 1.0f,
              float pan = 0.0f,
              bool loop = false);

    /// Pause playback of all sounds in pool.
    void pause();
    /// Pause playback of sound with id.
    void pause(long id);

    /// Pause all sounds and wipe pool.
    void stop();
    /// Pause only sound with id and remove it from pool.
    void stop(long id);

    /// Resume playback of all sounds in pool.
    void resume();
    /// Resume playback of sound with id.
    void resume(long id);

    /// Set pan effect of sound with id.
    void pan(long id, float value);
    /// Set speed of sound with id.
    void speed(long id, float value);
    /// Set volume of sound with id.
    void volume(long id, float value);
    /// Set if sound with id is looping.
    void looping(long id, bool loop);

public:
    // renderable_audio interface
    void render(int16_t *stream, uint32_t frames);

private:
    struct sound {
        uint32_t m_cur_frame;
        bool m_paused;
        long m_id;
        float m_volume;
        bool m_looping;
        pan_effect m_pan;
        resampler m_resampler;
    };

    sound gen_sound(float volume, float pan, float speed, bool loop);
    void do_by_id(long, const std::function<void(std::vector<sound>::iterator)> &);

    std::vector<sound> m_sounds;
    long m_last_id;

    uint32_t m_frames;
    int8_t m_channels;
    std::vector<float> m_pcm, m_sample_buffer;

    std::atomic_flag m_rendering_flag;
};