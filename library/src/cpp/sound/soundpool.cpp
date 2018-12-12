#include "soundpool.hpp"
#include <iterator>
#include <algorithm>
#include "../utility/log.hpp"

soundpool::soundpool(const data&& p_pcm, int8_t p_channels)
    : m_channels(p_channels)
    , m_frames(p_pcm.size() / p_channels)
    , m_pcm(std::move(p_pcm))
    , m_last_id(0) { }

void soundpool::do_by_id(long p_id, std::function<void(soundpool::sound&)> p_callback) {
    auto iter = std::find_if(m_sounds.begin(), m_sounds.end(), [p_id](const soundpool::sound& p_sound) {
        return p_sound.m_id == p_id;
    });
    if(iter != m_sounds.end()) {
        p_callback(*iter);
    }
}

soundpool::sound soundpool::gen_sound(float p_volume) {
    return sound {
        .m_volume = p_volume,
        .m_id = ++m_last_id,
        .m_paused = false,
        .m_cur_frame = 0
    };
}

long soundpool::play(float p_volume) {
    m_sounds.push_front(gen_sound(p_volume));
    return m_sounds.front().m_id;
}

void soundpool::pause() {
    for(auto& sound : m_sounds) { sound.m_paused = true; }
}

void soundpool::pause(long p_id) {
    do_by_id(p_id, [](sound& p_sound) { p_sound.m_paused = true; });
}

void soundpool::resume() {
    for(auto& sound : m_sounds) { sound.m_paused = false; }
}

void soundpool::resume(long p_id) {
    do_by_id(p_id, [](sound& p_sound) { p_sound.m_paused = false; });
}

void soundpool::stop() {
    for(auto& sound : m_sounds) {
        sound.m_cur_frame = 0;
        sound.m_paused = true;
    }
}

void soundpool::stop(long p_id) {
    do_by_id(p_id, [](sound& p_sound) {
        p_sound.m_cur_frame = 0;
        p_sound.m_paused = true;
    });
}

void volume(long p_id, float p_volume) {
    do_by_id(p_id, [](sound& p_sound) { p_sound.m_volume = p_volume; }
}

void soundpool::render(int16_t* p_audio_data, int32_t p_num_frames) const {
    m_sounds.remove_if([&](sound& p_sound) {
        if(!p_sound.m_paused) {
            auto iter = std::next(m_pcm.begin(), p_sound.m_cur_frame * m_channels);
            const int size = std::min(p_num_frames, m_frames - p_sound.m_cur_frame);

            for(int frame = 0; frame < size; ++frame, ++p_sound.m_cur_frame) {
                for(int sample = 0; sample < m_channels; ++sample, std::advance(iter, 1)) {
                    p_audio_data[frame * m_channels + sample] += *iter * p_sound.m_volume;
                }
            }
        }

        return p_sound.m_cur_frame >= m_frames;
    });
}

bool soundpool::is_done() const {
    // TODO is this super wrong ?
    return false;
}

