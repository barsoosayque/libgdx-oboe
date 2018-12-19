#pragma once
#include "../audio/decoder.hpp"
#include "../audio/renderableaudio.hpp"
#include <thread>

class music: public renderable_audio {
    public:
        music(opensl::decoder&& p_decoder, int8_t p_channels);

        void render(int16_t* p_stream, int32_t p_frames);
        bool is_done() const;

        void play();
        void pause();
        void stop();

        void position(float p_position);
        float position();
        float duration();

        bool is_playing();
    private:
        void fill_second_buffer();
        void swap_buffers();

        bool m_playing;
        int m_cache_size;
        int8_t m_channels;
        opensl::decoder m_decoder;

        std::thread m_decoder_thread;

        int32_t m_current_frame;
        std::vector<int16_t> m_main_pcm;

        std::vector<int16_t> m_second_pcm;
};