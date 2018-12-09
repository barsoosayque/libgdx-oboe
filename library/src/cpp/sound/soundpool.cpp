#include "soundpool.hpp"
#include <iterator>
#include <algorithm>
#include "../utility/log.hpp"

soundpool::soundpool(SLEngineItf& p_engine, SLDataSource& p_source)
    : player_handle(p_engine, p_source)
    , m_channels(2)
    , m_size(0) {

    (*m_queue)->RegisterCallback(m_queue, [](const SLBufferQueueItf p_queue, void* p_data) {
        auto self = reinterpret_cast<soundpool*>(p_data);
        self->m_size += self->m_pcm_buffer.capacity();

        auto end = std::next(self->m_pcm_buffer.cbegin(), self->m_pcm_buffer.capacity());
        self->m_pcm_data.insert(self->m_pcm_data.end(), self->m_pcm_buffer.cbegin(), end);

        self->enqueue_new_data();
    }, this);

    // synchronously load all the data
    SLBufferQueueState state;
    enqueue_new_data();
    (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_PLAYING);
    (*m_queue)->GetState(m_queue, &state);
    while(state.count) {
        (*m_queue)->GetState(m_queue, &state);
    }
}

void soundpool::do_by_id(long p_id, std::function<void(soundpool::sound&)> p_callback) {
    auto iter = std::find_if(m_sounds.begin(), m_sounds.end(), [p_id](const soundpool::sound& p_sound) {
        return p_sound.m_id == p_id;
    });
    if(iter != m_sounds.end()) {
        p_callback(*iter);
    }
}

long soundpool::play() {
    sound new_sound = {
        .m_volume = 1.0f,
        .m_id = m_last_id + 1,
        .m_is_paused = false,
        .m_cur_sample = 0
    };
    m_last_id = new_sound.m_id;
    m_sounds.push_front(new_sound);
    return new_sound.m_id;
}

long soundpool::play(float p_volume) {
    sound new_sound = {
        .m_volume = p_volume,
        .m_id = m_last_id + 1,
        .m_is_paused = false,
        .m_cur_sample = 0
    };
    m_last_id = new_sound.m_id;
    m_sounds.push_front(new_sound);
    return new_sound.m_id;
}

void soundpool::pause() {
    for(auto& sound : m_sounds) {
        sound.m_is_paused = true;
    }
}

void soundpool::pause(long p_id) {
    do_by_id(p_id, [](sound& p_sound) { p_sound.m_is_paused = true; });
}

void soundpool::stop() {
    for(auto& sound : m_sounds) {
        sound.m_cur_sample = 0;
        sound.m_is_paused = true;
    }
}

void soundpool::stop(long p_id) {
    do_by_id(p_id, [](sound& p_sound) {
        p_sound.m_cur_sample = 0;
        p_sound.m_is_paused = true;
    });
}

void soundpool::render(int16_t* p_audio_data, int32_t p_num_frames) const {
    m_sounds.remove_if([&](sound& p_sound) {
        if(!p_sound.m_is_paused) {
            auto iter = std::next(m_pcm_data.begin(), p_sound.m_cur_sample);
            const int32_t size = std::min(p_num_frames * m_channels, m_size - p_sound.m_cur_sample);

            for(int32_t i = 0; i < size; ++i, std::advance(iter, 1), ++p_sound.m_cur_sample) {
                p_audio_data[i] += *iter;
            }
        }

        return p_sound.m_cur_sample >= m_size;
    });
}

bool soundpool::is_done() const {
    return false;
}

