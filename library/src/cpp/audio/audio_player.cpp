#include "audio_player.hpp"
#include "../utility/log.hpp"
#include <algorithm>
#include <limits>

namespace {
int64_t k_limit_down = std::numeric_limits<int16_t>::min();
int64_t k_limit_up = std::numeric_limits<int16_t>::max();
}

audio_player::audio_player()
    : m_engine(oboe_engine::mode::async_writing)
    , m_volume(1.0f)
    , m_rendering_flag(false) {
    m_engine.set_on_async_write([this](uint32_t num_frames) -> const std::vector<int16_t>& {
        return generate_audio(num_frames);
    });
}

void audio_player::play_audio(const std::shared_ptr<renderable_audio> &audio) {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));

    m_tracks.emplace_back(audio);

    m_rendering_flag.clear(std::memory_order_release);
}

const std::vector<int16_t>& audio_player::generate_audio(uint32_t num_frames) {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));

    m_pcm.clear();
    m_pcm.resize(num_frames, 0);
    m_buffer.resize(m_pcm.size(), 0);

    int64_t prevaluated = 0;
    bool is_dirty = false;

    for (const auto &weak_track : m_tracks) {
        is_dirty |= weak_track.expired();
        if (auto track = weak_track.lock()) {
            std::fill(m_buffer.begin(), m_buffer.end(), 0);
            track->render(m_buffer.data(), num_frames / m_engine.channels());

            for (int i = 0; i < m_pcm.size(); ++i) {
                prevaluated = static_cast<int64_t>(m_pcm[i]) + static_cast<int64_t>(m_buffer[i]);
                m_pcm[i] = static_cast<int16_t>(std::clamp(prevaluated, k_limit_down, k_limit_up));
            }
        }
    }

    if (is_dirty) {
        m_tracks.erase(std::remove_if(m_tracks.begin(), m_tracks.end(),
                                      [](const std::weak_ptr<renderable_audio>& track) {
                                          return track.expired();
                                      }), m_tracks.end());
    }


    for (auto& pcm_bit : m_pcm) {
        pcm_bit = static_cast<int16_t>(static_cast<float>(pcm_bit) * m_volume);
    }

    m_rendering_flag.clear(std::memory_order_release);

    return m_pcm;
}
