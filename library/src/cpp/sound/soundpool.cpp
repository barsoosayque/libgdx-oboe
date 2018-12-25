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

soundpool::sound soundpool::gen_sound(float p_volume, float p_pan, float p_speed, bool p_loop) {
    return sound {
        .m_volume = p_volume,
        .m_id = ++m_last_id,
        .m_paused = false,
        .m_looping = p_loop,
        .m_speed = p_speed,
        .m_cur_frame = 0,
        .m_pan = pan_effect(p_pan)
    };
}

long soundpool::play(float p_volume, float p_pan, float p_speed, bool p_loop) {
    m_sounds.push_front(gen_sound(p_volume, p_pan, p_speed, p_loop));
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

void soundpool::volume(long p_id, float p_volume) {
    do_by_id(p_id, [p_volume](sound& p_sound) { p_sound.m_volume = p_volume; });
}

void soundpool::looping(long p_id, bool p_loop) {
    do_by_id(p_id, [p_loop](sound& p_sound) { p_sound.m_looping = p_loop; });
}

void soundpool::speed(long p_id, float p_speed) {
    do_by_id(p_id, [p_speed](sound& p_sound) { p_sound.m_speed = std::max(std::min(p_speed, 2.0f), 0.5f); });
}

void soundpool::pan(long p_id, float p_pan) {
    do_by_id(p_id, [p_pan](sound& p_sound) { p_sound.m_pan.pan(p_pan); });
}

void soundpool::render(int16_t* p_audio_data, int32_t p_num_frames) {
    m_sounds.remove_if([&](sound& p_sound) {
        if(!p_sound.m_paused) {
            auto iter = std::next(m_pcm.begin(), p_sound.m_cur_frame * m_channels);
            const int size = std::min(p_num_frames, m_frames - p_sound.m_cur_frame);
            p_sound.m_cur_frame += size * p_sound.m_speed;

            float speed_step = std::log2(p_sound.m_speed),
                  step_acc = 0.0f;

            for(int frame = 0; frame < size; ++frame) {
                for(int sample = 0; sample < m_channels; ++sample, std::advance(iter, 1)) {
                    p_audio_data[frame * m_channels + sample] += *iter * p_sound.m_volume * p_sound.m_pan.modulation(sample);
                }

                step_acc += p_sound.m_speed;
                if (int count = std::floor(step_acc)) {
                    step_acc -= count;
                    std::advance(iter, speed_step * m_channels);
                }
            }
        }

        if(p_sound.m_cur_frame >= m_frames && p_sound.m_looping) {
            p_sound.m_cur_frame = 0;
        }
        return p_sound.m_cur_frame >= m_frames;
    });
}

bool soundpool::is_done() const {
    // TODO is this super wrong ?
    return false;
}

