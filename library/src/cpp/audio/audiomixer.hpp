#pragma once

#include "../core/renderableaudio.hpp"
#include <memory>
#include <list>
#include <vector>

class audio_mixer: renderable_audio {
    public:
        /// Initialize mixer with specified buffer size and channel size
        audio_mixer(int32_t, int8_t);

        void play_audio(const renderable_audio&);
        void play_audio(renderable_audio*);
        void render(int16_t*, int32_t) const;
        bool is_done() const;

    private:
        const int8_t m_channels;
        mutable std::vector<int16_t> m_buffer;
        mutable std::list<std::shared_ptr<const renderable_audio>> m_tracks;
};