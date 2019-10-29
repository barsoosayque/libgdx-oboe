#include "soundpool.hpp"
#include <iterator>
#include <algorithm>
#include "../samplerate/pcmtypes.hpp"
#include "../utility/log.hpp"

soundpool::soundpool(const data& p_pcm, int8_t p_channels)
    : m_last_id(0)
    , m_frames(p_pcm.size() / p_channels)
    , m_channels(p_channels)
    , m_pcm(to_float(p_pcm))
    , m_rendering_flag(false)
{}

void soundpool::do_by_id(long p_id, std::function<void(soundpool::sound&)> p_callback) {
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    auto iter = std::find_if(m_sounds.begin(), m_sounds.end(), [p_id](const soundpool::sound& p_sound) {
        return p_sound.m_id == p_id;
    });
    if(iter != m_sounds.end()) {
        p_callback(*iter);
    }
    m_rendering_flag.clear(std::memory_order_release);
}

soundpool::sound soundpool::gen_sound(float p_volume, float p_pan, float p_speed, bool p_loop) {
    return sound {
        .m_volume = p_volume,
        .m_id = ++m_last_id,
        .m_paused = false,
        .m_looping = p_loop,
        .m_cur_frame = 0,
        .m_pan = pan_effect(p_pan),
        .m_resampler = resampler(resampler::converter::zero_order_hold, m_channels, 1.f / std::clamp(p_speed, 0.5f, 2.0f))
    };
}

long soundpool::play(float p_volume, float p_pan, float p_speed, bool p_loop) {
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    m_sounds.emplace_back(gen_sound(p_volume, p_pan, p_speed, p_loop));
    long id = m_sounds.back().m_id;
    m_rendering_flag.clear(std::memory_order_release);
    return id;
}

void soundpool::pause() {
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    for(auto& sound : m_sounds) { sound.m_paused = true; }
    m_rendering_flag.clear(std::memory_order_release);
}

void soundpool::pause(long p_id) {
    do_by_id(p_id, [](sound& p_sound) { p_sound.m_paused = true; });
}

void soundpool::resume() {
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    for(auto& sound : m_sounds) { sound.m_paused = false; }
    m_rendering_flag.clear(std::memory_order_release);
}

void soundpool::resume(long p_id) {
    do_by_id(p_id, [](sound& p_sound) { p_sound.m_paused = false; });
}

void soundpool::stop() {
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    for(auto& sound : m_sounds) {
        sound.m_cur_frame = 0;
        sound.m_paused = true;
    }
    m_rendering_flag.clear(std::memory_order_release);
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
    do_by_id(p_id, [p_speed](sound& p_sound) {
        auto speed = std::clamp(p_speed, 0.5f, 2.0f);
        p_sound.m_resampler.ratio(1.f / speed);
    });
}

void soundpool::pan(long p_id, float p_pan) {
    do_by_id(p_id, [p_pan](sound& p_sound) { p_sound.m_pan.pan(p_pan); });
}

void soundpool::render(int16_t* p_audio_data, int32_t p_num_frames) {
    static int limit_down = std::numeric_limits<int16_t>::min(),
               limit_up = std::numeric_limits<int16_t>::max();

    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    int prevaluated = 0;
    m_sample_buffer.reserve(p_num_frames * m_channels + 16);
    for(auto it = m_sounds.begin(); it != m_sounds.end();) {
        if(!it->m_paused) {
            auto iter = std::next(m_pcm.cbegin(), it->m_cur_frame * m_channels);
            const int size = std::min(p_num_frames, m_frames - it->m_cur_frame);

            int used_frames = it->m_resampler.process(iter, m_pcm.cend(), m_sample_buffer.begin(), size);

            iter = m_sample_buffer.begin();
            auto end = std::next(iter, size * m_channels);
            for (int i = 0; iter != end; ++iter, ++i) {
                prevaluated = static_cast<int>(p_audio_data[i]) + static_cast<int>(*iter * limit_up * it->m_volume * it->m_pan.modulation(i % m_channels));
                p_audio_data[i] = static_cast<int16_t>(std::clamp(prevaluated, limit_down, limit_up));
            }

            it->m_cur_frame += used_frames;
        }

        if(it->m_cur_frame >= m_frames) {
            if(it->m_looping) {
                it->m_cur_frame = 0;
                it->m_resampler.reset();
            } else {
                it = m_sounds.erase(it);
            }
        } else {
            it++;
        }
    }
    m_rendering_flag.clear(std::memory_order_release);
}

