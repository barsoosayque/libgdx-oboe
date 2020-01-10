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
        soundpool(const data& pcm, int8_t channels);
        void render(int16_t* stream, int32_t frames);

        long play(float volume = 1.0f,
                  float speed = 1.0f,
                  float pan = 0.0f,
                  bool loop = false);

        void pause();
        void pause(long id);

        void stop();
        void stop(long id);

        void resume();
        void resume(long id);

        void pan(long id, float value);
        void speed(long id, float value);
        void volume(long id, float value);
        void looping(long id, bool loop);
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
        sound gen_sound(float volume, float pan, float speed, bool loop);
        void do_by_id(long, std::function<void(std::vector<sound>::iterator)>);
        std::vector<sound> m_sounds;
        long m_last_id;

        int32_t m_frames;
        int8_t m_channels;
        std::vector<float> m_pcm, m_sample_buffer;

        std::atomic_flag m_rendering_flag;
};