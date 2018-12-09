#pragma once
#include <vector>
#include <list>
#include "../core/playerhandle.hpp"
#include "../core/renderableaudio.hpp"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <optional>
#include <functional>

class soundpool : player_handle, public renderable_audio {
    public:
        soundpool(SLEngineItf&, SLDataSource&);
        void render(int16_t*, int32_t) const;
        bool is_done() const;

        long play();
        long play(float);

        void pause();
        void pause(long);

        void stop();
        void stop(long);

    private:
        struct sound {
            float m_volume;
            long m_id;
            bool m_is_paused;
            int32_t m_cur_sample;
        };
        void do_by_id(long, std::function<void(soundpool::sound&)>);

        long m_last_id;
        int32_t m_size;
        int8_t m_channels;
        mutable std::list<sound> m_sounds;
        std::vector<int16_t> m_pcm_data;
        const int16_t* dat;
};