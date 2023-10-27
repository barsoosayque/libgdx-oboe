#include "music.hpp"
#include <iterator>
#include <cmath>

music::music(std::unique_ptr<audio_decoder> &&decoder, int8_t channels)
        : m_pan(0)
        , m_looping(false)
        , m_cache_size(16 * 1024 * channels)
        , m_volume(1)
        , m_channels(channels)
        , m_decoder(std::move(decoder))
        , m_current_frame(0)
        , m_buffer_swap(false)
        , m_executor([&]() { fill_second_buffer(); }) {
    m_main_pcm.reserve(m_cache_size);
    stop();
}

void music::fill_second_buffer() {
    m_decoder->decode(m_cache_size).swap(m_buffer_pcm);
}

void music::swap_buffers() {
    m_main_pcm.swap(m_buffer_pcm);
    m_eof = m_decoder->is_eof();
    m_current_frame = 0;
}

void music::play() {
    if (m_eof)
        stop();
    m_playing = true;
}

void music::pause() {
    m_playing = false;
}

void music::stop() {
    m_playing = false;
    m_eof = false;
    position(0);
}

void music::position(float position) {
    while (m_buffer_swap.test_and_set(std::memory_order_acquire));
    m_executor.wait();
    m_decoder->seek(position);
    m_position = position;
    fill_second_buffer();
    swap_buffers();
    m_executor.queue();
    m_buffer_swap.clear(std::memory_order_release);
}

void music::raw_render(int16_t *stream, uint32_t frames) {
    if (!m_playing)
        return;

    auto iter = std::next(m_main_pcm.begin(), m_current_frame * m_channels);
    auto iter_rem = std::distance(iter, m_main_pcm.end());
    int size = frames * m_channels;
    if (size > iter_rem) {
        size = iter_rem;
    }
    for (int sample = 0; sample < size; ++sample, std::advance(iter, 1)) {
        stream[sample] += *iter * m_volume * m_pan.modulation(sample % m_channels);
    }

    // TODO remove hard-coded stuff
    m_position += frames / 44100.0f;
    m_current_frame += frames;
}

void music::render(int16_t *stream, uint32_t frames) {
    if (!m_playing)
        return;
    while (m_buffer_swap.test_and_set(std::memory_order_acquire));

    int32_t frames_in_pcm = m_main_pcm.size() / m_channels;
    uint32_t frames_to_process = std::clamp(frames_in_pcm - m_current_frame, 0,
                                            static_cast<int32_t>(frames));

    raw_render(stream, frames_to_process);
    m_buffer_swap.clear(std::memory_order_release);

    if (frames_to_process < frames) {
        bool end = m_eof && m_current_frame >= frames_in_pcm;
        if (end) {
            m_playing = m_looping;
            m_position = 0;
            if (m_on_complete && !m_looping)
                m_on_complete();
        }

        // wait for buffer in case there was no position reset
        m_executor.wait();
        swap_buffers();
        if (m_playing) {
            if (m_looping && m_decoder->is_eof()) {
                m_decoder->seek(0);
            }
            m_executor.queue();
        }

        // render additional pcm to fill full stream
        while (m_buffer_swap.test_and_set(std::memory_order_acquire));
        int16_t remain = frames - frames_to_process;
        raw_render(stream + frames_to_process * m_channels, remain);
        m_buffer_swap.clear(std::memory_order_release);
    }
}
