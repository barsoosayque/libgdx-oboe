#pragma once
#include <vector>
#include <list>
#include "../audio/renderableaudio.hpp"
#include <functional>

class soundpool: public renderable_audio {
    public:
        using data = std::vector<int16_t>;
        soundpool(const data&& p_pcm, int8_t p_channels);
        void render(int16_t* p_stream, int32_t p_frames) const;
        bool is_done() const;

        long play(float p_volume = 1.0f, bool p_loop = false);

        void pause();
        void pause(long p_id);

        void stop();
        void stop(long p_id);

        void resume();
        void resume(long p_id);

        void volume(long p_id, float p_volume);
        void looping(long p_id, bool p_loop);
    private:
        struct sound {
            float m_volume;
            long m_id;
            bool m_paused;
            bool m_looping;
            int32_t m_cur_frame;
        };
        sound gen_sound(float p_volume = 1.0f, bool p_loop = false);
        void do_by_id(long, std::function<void(soundpool::sound&)>);
        mutable std::list<sound> m_sounds;
        long m_last_id;

        int32_t m_frames;
        int8_t m_channels;
        std::vector<int16_t> m_pcm;
};