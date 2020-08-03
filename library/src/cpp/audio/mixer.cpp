#include "mixer.hpp"
#include <algorithm>
#include <limits>

mixer::mixer(int32_t buffer_size, int8_t channels)
    : m_volume(1)
    , m_channels(channels)
    , m_buffer(buffer_size)
    , m_rendering_flag(false)
    {}

void mixer::resize_buffer(int32_t new_size) {
    m_buffer.reserve(new_size);
}

void mixer::play_audio(renderable_audio* track) {
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    m_tracks.emplace_back(track);
    m_rendering_flag.clear(std::memory_order_release);
}

void mixer::render(int16_t* audio_data, int32_t num_frames) {
    static int limit_down = std::numeric_limits<int16_t>::min(),
               limit_up = std::numeric_limits<int16_t>::max();

    std::fill(audio_data, audio_data + num_frames * m_channels, 0);
    int prevaluated = 0;
    bool is_dirty = false;
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    for(const auto& track : m_tracks) {
        is_dirty |= track == nullptr;
        if(track) {
            std::fill(m_buffer.begin(), m_buffer.begin() + num_frames * m_channels, 0);
            track->render(m_buffer.data(), num_frames);

            for(int i = 0; i < num_frames * m_channels; ++i) {
                prevaluated = static_cast<int>(audio_data[i]) + static_cast<int>(m_buffer[i]);
                audio_data[i] = static_cast<int16_t>(std::clamp(prevaluated, limit_down, limit_up));
            }
        }
    }
    if(is_dirty) {
        m_tracks.erase(std::remove_if(m_tracks.begin(), m_tracks.end(), [](const renderable_audio* track) {
            return !track;
        }), m_tracks.end());
    }
    m_rendering_flag.clear(std::memory_order_release);

    for (int j = 0; j < num_frames * m_channels; ++j) {
            audio_data[j] *= m_volume;
    }
}
