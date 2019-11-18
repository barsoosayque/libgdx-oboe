#pragma once
#include <vector>
#include "../audio/pan_effect.hpp"
#include "../audio/renderableaudio.hpp"
#include "../samplerate/resampler.hpp"
#include <functional>
#include <atomic>

class soundpool: public renderable_audio {
    public:
        using data = std::vector<int16_t>;
        soundpool(const data& p_pcm, int8_t p_channels);
        void render(int16_t* p_stream, int32_t p_frames);

        long play(float p_volume = 1.0f,
                  float p_pan = 0.0f,
                  float p_speed = 1.0f,
                  bool p_loop = false);

        void pause();
        void pause(long p_id);

        void stop();
        void stop(long p_id);

        void resume();
        void resume(long p_id);

        void pan(long p_id, float p_pan);
        void speed(long p_id, float p_speed);
        void volume(long p_id, float p_volume);
        void looping(long p_id, bool p_loop);
    private:
        struct sound {
            int32_t m_cur_frame;
            bool m_paused;
            long m_id;
            float m_volume;
            bool m_looping;
            pan_effect m_pan;
            resampler m_resampler;
        };
        sound gen_sound(float p_volume, float p_pan, float p_speed, bool p_loop);
        void do_by_id(long, std::function<void(std::vector<sound>::iterator)>);
        std::vector<sound> m_sounds;
        long m_last_id;

        int32_t m_frames;
        int8_t m_channels;
        std::vector<float> m_pcm, m_sample_buffer;

        std::atomic_flag m_rendering_flag;
};