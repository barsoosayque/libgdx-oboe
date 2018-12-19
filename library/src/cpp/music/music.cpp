#include "music.hpp"
#include "../utility/log.hpp"
#include <iterator>
#include <cmath>

music::music(opensl::decoder&& p_decoder, int8_t p_channels)
    : m_decoder(std::move(p_decoder))
    , m_channels(p_channels)
    , m_cache_size(16 * 1024 * 2)
    , m_current_frame(0) {
    if(!m_decoder.is_opened()) {
        error("Trying to create music with decoder that is not open.");
    }
    m_second_pcm.reserve(m_cache_size);
    m_main_pcm.reserve(m_cache_size);
    fill_second_buffer();
    swap_buffers();
    fill_second_buffer();
}

void music::fill_second_buffer() {
    m_decoder.request_more(m_cache_size).swap(m_second_pcm);
}

void music::swap_buffers() {
    m_main_pcm.swap(m_second_pcm);
    m_second_pcm.clear();
}

void music::play() {
    m_playing = true;
}

void music::pause() {
    m_playing = false;
}

void music::stop() {
    m_playing = false;
    position(0.0f);
    m_current_frame = 0;
}

bool music::is_playing() {
    return m_playing;
}

void music::position(float p_position) {
    m_decoder.position(p_position);
}

float music::position() {
    // FIXME music poistion, not decoder position
    return m_decoder.position();
}

float music::duration() {
    return m_decoder.content_duration();
}

void music::render(int16_t* p_stream, int32_t p_frames) {
    int32_t frames_in_pcm = m_cache_size / m_channels;
    bool perform_swap = false;

    if((m_current_frame + p_frames) >= frames_in_pcm) {
        if(m_decoder_thread.joinable()) m_decoder_thread.join();
        perform_swap = true;
    }

    if(m_playing) {
        auto iter = std::next(m_main_pcm.begin(), m_current_frame * m_channels);

        for(int frame = 0; frame < p_frames; ++frame, ++m_current_frame) {
            if(m_current_frame >= frames_in_pcm) {
                m_current_frame = 0;
                iter = m_second_pcm.begin();
            }
            for(int sample = 0; sample < m_channels; ++sample, std::advance(iter, 1)) {
                p_stream[frame * m_channels + sample] += *iter;
            }
        }
    }

    if(perform_swap) {
        swap_buffers();
        if(m_current_frame >= frames_in_pcm) {
            m_current_frame -= frames_in_pcm;
        }
        m_decoder_thread = std::thread(&music::fill_second_buffer, this);
    }
}

bool music::is_done() const {
    // TODO seems like a hack
    return false;
}