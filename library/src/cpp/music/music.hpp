#pragma once
#include "../opensl/decoder.hpp"
#include "../audio/renderableaudio.hpp"

class music: public renderable_audio {
    public:
        music(opensl::decoder&& p_decoder);

        void render(int16_t* p_stream, int32_t p_frames) const;
        bool is_done() const;

        float position();
        float duration();
    private:
        opensl::decoder m_decoder;
};