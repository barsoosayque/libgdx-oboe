#include "music.hpp"
#include "../utility/log.hpp"
#include <iterator>
#include <cmath>

music::music(audio_decoder&& p_decoder, int8_t p_channels)
    : m_decoder(std::move(p_decoder))
    , m_looping(false)
    , m_pan(0.0f)
    , m_volume(1.0f)
    , m_channels(p_channels)
    , m_cache_size(16 * 1024 * p_channels)
    , m_current_frame(0) {
    m_second_pcm.reserve(m_cache_size);
    m_main_pcm.reserve(m_cache_size);
    stop();
}

void music::fill_second_buffer() {
    auto pcm = m_decoder.decode(m_cache_size);
    pcm.swap(m_second_pcm);
}

void music::swap_buffers() {
    m_main_pcm.swap(m_second_pcm);
    m_second_pcm.clear();
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
    m_current_frame = 0;
    position(0.0f);
}

bool music::is_playing() {
    return m_playing;
}

void music::position(float p_position) {
    if(m_decoder_thread.joinable()) m_decoder_thread.join();
    m_position = p_position;
    m_decoder.seek(p_position);
    fill_second_buffer();
    swap_buffers();
    fill_second_buffer();
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

    int32_t frames_in_pcm = m_main_pcm.size() / m_channels;
    bool perform_swap = false;
    bool last_buffer = false;

    if((m_current_frame + p_frames) >= frames_in_pcm) {
        if(m_decoder_thread.joinable()) m_decoder_thread.join();
        if(m_second_pcm.size() < m_cache_size) {
            last_buffer = true;
            if(m_looping) {
                m_decoder.seek(0.0f);
            }
        }
        perform_swap = true;
    }

    int32_t max_frames = p_frames;
    if(!m_looping && last_buffer) {
        max_frames = std::min(max_frames, frames_in_pcm - m_current_frame);
        m_playing = max_frames >= p_frames;
        m_eof = !m_playing;
    }

    auto iter = std::next(m_main_pcm.begin(), m_current_frame * m_channels);

    for(int frame = 0; frame < max_frames; ++frame, ++m_current_frame) {
        if(m_current_frame >= frames_in_pcm) {
            m_current_frame = 0;
            iter = m_second_pcm.begin();
            if(last_buffer) {
                m_position = -frame;
            }
        }
        for(int sample = 0; sample < m_channels; ++sample, std::advance(iter, 1)) {
            p_stream[frame * m_channels + sample] += *iter * m_volume * m_pan.modulation(sample);
        }
    }

    if(perform_swap) {
        swap_buffers();
        if(m_current_frame >= frames_in_pcm) {
            m_current_frame -= frames_in_pcm;
        }
        m_decoder_thread = std::thread(&music::fill_second_buffer, this);
    }

    // TODO remove hard-coded stuff
    m_position += max_frames / 44100.0f;

    if (!m_playing && m_on_complete) {
        m_on_complete();
    }
}

bool music::is_done() const {
    // TODO seems like a hack
    return false;
}
