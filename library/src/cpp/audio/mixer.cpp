#include "mixer.hpp"
#include <algorithm>

mixer::mixer(int32_t p_buffer_size, int8_t p_channels)
    : m_volume(1)
    , m_channels(p_channels)
    , m_buffer(p_buffer_size)
    {}

void mixer::resize_buffer(int32_t p_new_size) {
    m_buffer.reserve(p_new_size);
}

void mixer::play_audio(std::shared_ptr<renderable_audio> p_track) {
    m_tracks.emplace_back(p_track);
}

void mixer::render(int16_t* p_audio_data, int32_t p_num_frames) {
    for (int j = 0; j < p_num_frames * m_channels; ++j) {
            p_audio_data[j] = 0;
    }

    m_tracks.remove_if([&](const std::weak_ptr<renderable_audio>& track) {
        if(auto ptr = track.lock()) {
            std::fill(m_buffer.begin(), m_buffer.begin() + m_buffer.capacity(), 0);
            ptr->render(m_buffer.data(), p_num_frames);

            for(int i = 0; i < p_num_frames * m_channels; ++i) {
                p_audio_data[i] += m_buffer[i] * m_volume;
            }

            return false;
        } else {
            return true;
        }
    });
}
