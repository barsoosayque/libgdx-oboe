#pragma once

#include "renderableaudio.hpp"
#include <memory>
#include <list>
#include <vector>

class mixer: renderable_audio {
    public:
        /// Initialize mixer with specified buffer size and channel size
        mixer(int32_t, int8_t);

        void play_audio(std::shared_ptr<renderable_audio>);
        void render(int16_t*, int32_t);

        float m_volume;
    private:
        const int8_t m_channels;
        mutable std::vector<int16_t> m_buffer;
        mutable std::list<std::weak_ptr<renderable_audio>> m_tracks;
};