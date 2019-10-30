#include "music.hpp"
#include <iterator>
#include <cmath>

music::music(std::shared_ptr<audio_decoder> p_decoder, int8_t p_channels)
    : m_pan(0)
    , m_looping(false)
    , m_cache_size(16 * 1024 * p_channels)
    , m_volume(1)
    , m_channels(p_channels)
    , m_decoder(p_decoder)
    , m_current_frame(0)
    , m_buffer_swap(false)
    , m_executor([&]() { fill_second_buffer(); }) {
    m_main_pcm.reserve(m_cache_size);
    stop();
}

void music::fill_second_buffer() {
    m_decoder->decode(m_cache_size);
}

void music::swap_buffers() {
    m_main_pcm.swap(m_decoder->m_buffer);
    m_eof = m_decoder->m_eof;
    m_current_frame = 0;
}

void music::play() {
    if(m_eof) stop();
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

bool music::is_playing() {
    return m_playing;
}

void music::position(float p_position) {
    while(m_buffer_swap.test_and_set(std::memory_order_acquire));
    m_decoder->seek(p_position);
    m_position = p_position;
    fill_second_buffer();
    swap_buffers();
    m_executor.queue();
    m_buffer_swap.clear(std::memory_order_release);
}

float music::position() {
    return m_position;
}

void music::volume(float p_volume) {
    m_volume = std::min(std::max(0.0f, p_volume), 1.0f);
}

float music::volume() {
    return m_volume;
}

bool music::is_looping() {
    return m_looping;
}

void music::is_looping(bool p_loop) {
    m_looping = p_loop;
}

void music::on_complete(std::function<void()> p_callback) {
    m_on_complete = p_callback;
}

void music::pan(float p_pan) {
    m_pan.pan(p_pan);
}

void music::raw_render(int16_t* p_stream, int32_t p_frames) {
    if(!m_playing) return;

    auto iter = std::next(m_main_pcm.begin(), m_current_frame * m_channels);
    int size = p_frames * m_channels;
    for(int sample = 0; sample < size; ++sample, std::advance(iter, 1)) {
        p_stream[sample] += *iter * m_volume * m_pan.modulation(sample % m_channels);
    }

    // TODO remove hard-coded stuff
    m_position += p_frames / 44100.0f;
    m_current_frame += p_frames;
}

void music::render(int16_t* p_stream, int32_t p_frames) {
    if(!m_playing) return;
    while(m_buffer_swap.test_and_set(std::memory_order_acquire));

    int32_t frames_in_pcm = m_main_pcm.size() / m_channels;
    int32_t frames_to_process = std::min(p_frames, frames_in_pcm - m_current_frame);

    raw_render(p_stream, frames_to_process);
    m_buffer_swap.clear(std::memory_order_release);

    if (frames_to_process < p_frames) {
        bool end = m_eof && m_current_frame >= frames_in_pcm;
        if (end) {
            m_playing = m_looping;
            m_position = 0;
            if (m_on_complete && !m_looping) m_on_complete();
        }

        // wait for buffer in case there was no position reset
        m_executor.wait();
        swap_buffers();
        if(m_playing) {
            if (m_looping && m_decoder->m_eof) {
                m_decoder->seek(0);
            }
            m_executor.queue();
        }

        // render additional pcm to fill full stream
        while(m_buffer_swap.test_and_set(std::memory_order_acquire));
        int16_t remain = p_frames - frames_to_process;
        raw_render(p_stream + frames_to_process * m_channels, remain);
        m_buffer_swap.clear(std::memory_order_release);
    }
}
