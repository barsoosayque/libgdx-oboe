#pragma once
#include "../audio/renderableaudio.hpp"

class music: public renderable_audio {
    public:
        void render(int16_t* p_stream, int32_t p_frames) const;
        bool is_done() const;
};