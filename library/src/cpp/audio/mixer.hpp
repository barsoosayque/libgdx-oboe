#pragma once

#include "renderableaudio.hpp"
#include <memory>
#include <vector>
#include <atomic>

class mixer: renderable_audio {
    public:
        /// Initialize mixer with specified buffer size and channel size
        mixer(int32_t, int8_t);
        ~mixer();

        void resize_buffer(int32_t);
        void play_audio(renderable_audio*);
        void render(int16_t*, int32_t);

        float m_volume;
    private:
        const int8_t m_channels;
        mutable std::vector<int16_t> m_buffer;
        mutable std::vector<renderable_audio*> m_tracks;
        std::atomic_flag m_rendering_flag;
};