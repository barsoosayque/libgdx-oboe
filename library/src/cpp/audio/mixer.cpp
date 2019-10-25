#include "mixer.hpp"
#include <algorithm>
#include <limits>

mixer::mixer(int32_t p_buffer_size, int8_t p_channels)
    : m_volume(1)
    , m_channels(p_channels)
    , m_buffer(p_buffer_size)
    {}

void mixer::resize_buffer(int32_t p_new_size) {
    m_buffer.reserve(p_new_size);
}

void mixer::play_audio(std::shared_ptr<renderable_audio> p_track) {
    while(!m_rendering_flag.test_and_set(std::memory_order_acquire));
    m_tracks.emplace_back(p_track);
    m_rendering_flag.clear();
}

void mixer::render(int16_t* p_audio_data, int32_t p_num_frames) {
    static int limit_down = std::numeric_limits<int16_t>::min(),
               limit_up = std::numeric_limits<int16_t>::max();

    for (int j = 0; j < p_num_frames * m_channels; ++j) {
            p_audio_data[j] = 0;
    }

    int prevaluated = 0;
    while(!m_rendering_flag.test_and_set(std::memory_order_acquire));
    for(const auto& track : m_tracks) {
        if(auto ptr = track.lock()) {
            std::fill(m_buffer.begin(), m_buffer.begin() + m_buffer.capacity(), 0);
            ptr->render(m_buffer.data(), p_num_frames);

            for(int i = 0; i < p_num_frames * m_channels; ++i) {
                prevaluated = static_cast<int>(p_audio_data[i]) + static_cast<int>(m_buffer[i]);
                p_audio_data[i] = static_cast<int16_t>(std::clamp(prevaluated, limit_down, limit_up));
            }
        }
    }
    m_tracks.erase(std::remove_if(m_tracks.begin(), m_tracks.end(), [](std::weak_ptr<renderable_audio> track) {
        return track.expired();
    }), m_tracks.end());
    m_rendering_flag.clear();

    for (int j = 0; j < p_num_frames * m_channels; ++j) {
            p_audio_data[j] *= m_volume;
    }
}
