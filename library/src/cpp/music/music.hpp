#pragma once

#include "../mediacodec/audio_decoder.hpp"
#include "../audio/renderable_audio.hpp"
#include "../audio/pan_effect.hpp"
#include "../utility/executor.hpp"
#include <atomic>

class music : public renderable_audio {
public:
    music(std::unique_ptr<audio_decoder> &&decoder, int8_t channels);

    void render(int16_t *stream, uint32_t frames);

    void play();
    void pause();
    void stop();

    void volume(float volume);
    float volume() const;

    void pan(float pan);

    void position(float position);
    float position() const;

    bool is_looping() const;
    void is_looping(bool loop);
    bool is_playing() const;

    void on_complete(const std::function<void()> &callback);
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