#pragma once
#include "../mediacodec/audio_decoder.hpp"
#include "../audio/renderableaudio.hpp"
#include <thread>

class music: public renderable_audio {
    public:
        music(audio_decoder&& p_decoder, int8_t p_channels);

        void render(int16_t* p_stream, int32_t p_frames);
        bool is_done() const;

        void play();
        void pause();
        void stop();

        void volume(float p_volume);
        float volume();

        void position(float p_position);
        float position();

        bool is_looping();
        void is_looping(bool p_loop);
        bool is_playing();

        void on_complete(std::function<void()> p_callback);
    private:
        void fill_second_buffer();
        void swap_buffers();

        bool m_playing,
             m_looping,
             m_eof;
        int m_cache_size;
        float m_position,
              m_volume;
        std::function<void()> m_on_complete;
        int8_t m_channels;
        audio_decoder m_decoder;
        std::thread m_decoder_thread;

        int32_t m_current_frame;
        std::vector<int16_t> m_main_pcm;
        std::vector<int16_t> m_second_pcm;
};