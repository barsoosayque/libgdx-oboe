#pragma once
#include "../mediacodec/audio_decoder.hpp"
#include "../audio/renderableaudio.hpp"
#include "../audio/pan_effect.hpp"
#include "../utility/executor.hpp"
#include <atomic>

class music: public renderable_audio {
    public:
        music(std::shared_ptr<audio_decoder> p_decoder, int8_t p_channels);

        void render(int16_t* p_stream, int32_t p_frames);

        void play();
        void pause();
        void stop();

        void volume(float p_volume);
        float volume();

        void pan(float p_pan);

        void position(float p_position);
        float position();

        bool is_looping();
        void is_looping(bool p_loop);
        bool is_playing();

        void on_complete(std::function<void()> p_callback);
    private:
        void fill_second_buffer();
        void swap_buffers();

        inline void raw_render(int16_t* p_stream, int32_t p_frames);

        pan_effect m_pan;
        bool m_playing, m_looping, m_eof;
        int m_cache_size;
        float m_position, m_volume;
        std::function<void()> m_on_complete;
        int8_t m_channels;
        std::shared_ptr<audio_decoder> m_decoder;

        int32_t m_current_frame;
        std::vector<int16_t> m_main_pcm;

        std::atomic_flag m_buffer_swap;
        executor m_executor;
};