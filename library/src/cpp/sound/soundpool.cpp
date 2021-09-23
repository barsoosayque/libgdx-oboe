#include "soundpool.hpp"
#include <iterator>
#include <algorithm>
#include "../samplerate/pcmtypes.hpp"
#include "../utility/log.hpp"

soundpool::soundpool(const data &pcm, int8_t channels)
        : m_last_id(0), m_frames(pcm.size() / channels), m_channels(channels), m_pcm(to_float(pcm)),
          m_rendering_flag(false) {}

void soundpool::do_by_id(long id, const std::function<void(
        std::vector<soundpool::sound>::iterator)> &callback) {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));
    auto iter = std::find_if(m_sounds.begin(), m_sounds.end(), [id](const soundpool::sound &sound) {
        return sound.m_id == id;
    });
    if (iter != m_sounds.end()) {
        callback(iter);
    }
    m_rendering_flag.clear(std::memory_order_release);
}

soundpool::sound soundpool::gen_sound(float volume, float pan, float speed, bool loop) {
    return sound{
            .m_cur_frame = 0,
            .m_paused = false,
            .m_id = ++m_last_id,
            .m_volume = volume,
            .m_looping = loop,
            .m_pan = pan_effect(pan),
            .m_resampler = resampler(resampler::converter::zero_order_hold, m_channels,
                                     1.f / std::clamp(speed, 0.5f, 2.0f))
    };
}

long soundpool::play(float volume, float speed, float pan, bool loop) {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));
    m_sounds.emplace_back(gen_sound(volume, pan, speed, loop));
    long id = m_sounds.back().m_id;
    m_rendering_flag.clear(std::memory_order_release);
    return id;
}

void soundpool::pause() {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));
    for (auto &sound : m_sounds) { sound.m_paused = true; }
    m_rendering_flag.clear(std::memory_order_release);
}

void soundpool::pause(long id) {
    do_by_id(id, [](auto sound) { sound->m_paused = true; });
}

void soundpool::resume() {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));
    for (auto &sound : m_sounds) { sound.m_paused = false; }
    m_rendering_flag.clear(std::memory_order_release);
}

void soundpool::resume(long id) {
    do_by_id(id, [](auto sound) { sound->m_paused = false; });
}

void soundpool::stop() {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));
    m_sounds.clear();
    m_rendering_flag.clear(std::memory_order_release);
}

void soundpool::stop(long id) {
    do_by_id(id, [&](auto sound) {
        m_sounds.erase(sound);
    });
}

void soundpool::volume(long id, float value) {
    do_by_id(id, [value](auto sound) { sound->m_volume = value; });
}

void soundpool::looping(long id, bool loop) {
    do_by_id(id, [loop](auto sound) { sound->m_looping = loop; });
}

void soundpool::speed(long id, float value) {
    do_by_id(id, [value](auto sound) {
        float safe_value = std::clamp(value, 0.5f, 2.0f);
        sound->m_resampler.ratio(1.f / safe_value);
    });
}

void soundpool::pan(long id, float value) {
    do_by_id(id, [value](auto sound) { sound->m_pan.pan(value); });
}

void soundpool::render(int16_t *audio_data, int32_t num_frames) {
    static int limit_down = std::numeric_limits<int16_t>::min(),
            limit_up = std::numeric_limits<int16_t>::max();

    while (m_rendering_flag.test_and_set(std::memory_order_acquire));
    int prevaluated = 0;
    m_sample_buffer.reserve(num_frames * m_channels + 16);
    for (auto it = m_sounds.begin(); it != m_sounds.end();) {
        if (!it->m_paused) {
            auto iter = std::next(m_pcm.cbegin(), it->m_cur_frame * m_channels);
            const int size = std::min(num_frames, m_frames - it->m_cur_frame);

            int used_frames = it->m_resampler.process(iter, m_pcm.cend(), m_sample_buffer.begin(),
                                                      size);

            auto buffer_iter = m_sample_buffer.begin();
            auto end = std::next(buffer_iter, size * m_channels);
            for (int i = 0; buffer_iter != end; ++buffer_iter, ++i) {
                prevaluated = static_cast<int>(audio_data[i]) +
                              static_cast<int>(*buffer_iter * limit_up * it->m_volume *
                                               it->m_pan.modulation(i % m_channels));
                audio_data[i] = static_cast<int16_t>(std::clamp(prevaluated, limit_down, limit_up));
            }

            it->m_cur_frame += used_frames;
        }

        if (it->m_cur_frame >= m_frames) {
            if (it->m_looping) {
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

