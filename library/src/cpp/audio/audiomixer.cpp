#include "audiomixer.hpp"
#include <algorithm>

audio_mixer::audio_mixer(int32_t p_buffer_size, int8_t p_channels)
    : m_buffer(p_buffer_size)
    , m_channels(p_channels) { }

void audio_mixer::play_audio(const renderable_audio& p_track) {
    m_tracks.emplace_back(&p_track);
}
void audio_mixer::play_audio(renderable_audio* p_track) {
    m_tracks.emplace_back(p_track);
}

bool audio_mixer::is_done() const {
    return m_tracks.empty();
}

void audio_mixer::render(int16_t* p_audio_data, int32_t p_num_frames) const {
    for (int j = 0; j < p_num_frames * m_channels; ++j) {
            p_audio_data[j] = 0;
    }

    m_tracks.remove_if([&](const std::shared_ptr<const renderable_audio>& track) {
        std::fill(m_buffer.begin(), m_buffer.begin() + m_buffer.capacity(), 0);
        track->render(m_buffer.data(), p_num_frames);

        for(int i = 0; i < p_num_frames * m_channels; ++i) {
            p_audio_data[i] += m_buffer[i];
        }

        return track->is_done();
    });
}
