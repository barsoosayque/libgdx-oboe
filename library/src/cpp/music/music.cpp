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
    , m_executor(std::bind(&music::fill_second_buffer, this)) {
    m_main_pcm.reserve(m_cache_size);
    stop();
}

void music::fill_second_buffer() {
    m_decoder->decode(m_cache_size);
}

void music::swap_buffers() {
    std::scoped_lock<std::recursive_mutex> lock(m_render_guard);
    m_main_pcm.swap(m_decoder->m_buffer);
    m_eof = m_decoder->m_eof;
}

void music::play() {
    if(m_eof) stop();
    m_playing = true;
}

void music::pause() {
    m_playing = false;
}

void music::stop() {
    std::scoped_lock<std::recursive_mutex> lock(m_render_guard);
    m_playing = false;
    m_eof = false;
    m_current_frame = 0;
    position(0);
}

bool music::is_playing() {
    return m_playing;
}

void music::position(float p_position) {
    std::scoped_lock<std::recursive_mutex> lock(m_render_guard);
    m_executor.stop();
    m_position = p_position;
    m_current_frame = 0;
    m_decoder->seek(p_position);
    fill_second_buffer();
    swap_buffers();
    m_executor.queue();
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

void music::render(int16_t* p_stream, int32_t p_frames) {
    if(!m_playing) return;
    std::scoped_lock<std::recursive_mutex> lock(m_render_guard);

    int32_t frames_in_pcm = m_main_pcm.size() / m_channels;
    int32_t frames_to_process = std::min(p_frames, frames_in_pcm - m_current_frame);

    auto iter = std::next(m_main_pcm.begin(), m_current_frame * m_channels);
    for(int frame = 0; frame < frames_to_process; ++frame, ++m_current_frame) {
        for(int sample = 0; sample < m_channels; ++sample, std::advance(iter, 1)) {
            p_stream[frame * m_channels + sample] += *iter * m_volume * m_pan.modulation(sample);
        }
    }
    // TODO remove hard-coded stuff
    m_position += frames_to_process / 44100.0f;

    if (m_eof && frames_to_process == 0) {
        if (m_on_complete && !m_looping) m_on_complete();
        m_playing = m_looping;
    }

    if (frames_to_process < p_frames) {
        m_executor.wait();
        swap_buffers();
        m_current_frame = 0;
        if(m_playing) {
            if (m_looping && m_decoder->m_eof) {
                m_position = 0;
                m_decoder->seek(0);
            }
            m_executor.queue();
        }

        if(int16_t remain = p_frames - frames_to_process) {
            render(p_stream + frames_to_process * m_channels, remain);
        }
    }
}
