#pragma once

#include "../mediacodec/audio_decoder.hpp"
#include "renderable_audio.hpp"
#include "pan_effect.hpp"
#include "../utility/executor.hpp"
#include <atomic>

/// Native implementation of streaming music.
/// Uses audio_decoder to render audio in chunks. Load PCM chunks
/// asynchronously, but may block if requested render.
class music : public renderable_audio {
public:
    using on_complete_t = std::function<void()>;

    /// Create a new music from a decoder and set number of channels.
    music(std::unique_ptr<audio_decoder> &&decoder, int8_t channels);

    /// Resume playback of the music stream.
    void play();
    /// Pause music stream (doesn't stop async loading).
    void pause();
    /// Pause music stream and resets position.
    void stop();
    /// Return true if music is playing.
    bool is_playing() const { return m_playing; }

    /// Set volume of music to value.
    void volume(float volume) { m_volume = std::clamp(volume, 0.0f, 1.0f); }
    /// Return current volume.
    float volume() const { return m_volume; }

    /// Set pan effect.
    void pan(float value) { m_pan.pan(value); }

    /// Set playback position (invalidates loading progress).
    void position(float position);
    /// Return current playback position.
    float position() const { return m_position; }

    /// Return true if looping.
    bool is_looping() const { return m_looping; }
    /// Set if music is looping.
    void is_looping(bool loop) { m_looping = loop; }

    /// Set callaback on completing music track (last frame rendered).
    void on_complete(on_complete_t&& callback) { m_on_complete = std::move(callback); }

public:
    // renderable_audio interface
    void render(int16_t *stream, uint32_t frames);

private:
    void fill_second_buffer();
    void swap_buffers();

    inline void raw_render(int16_t *stream, uint32_t frames);

    pan_effect m_pan;
    bool m_playing{}, m_looping, m_eof{};
    int m_cache_size;
    float m_position{}, m_volume;
    std::function<void()> m_on_complete;
    int8_t m_channels;
    std::unique_ptr<audio_decoder> m_decoder;

    int32_t m_current_frame;
    std::vector<int16_t> m_main_pcm;
    std::vector<int16_t> m_buffer_pcm;

    std::atomic_flag m_buffer_swap;
    executor m_executor;
};