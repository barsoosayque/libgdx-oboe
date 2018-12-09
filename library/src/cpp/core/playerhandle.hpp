#pragma once
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <vector>
#include <atomic>

class player_handle {
    public:
        player_handle(SLEngineItf&, SLDataSource&);
        virtual ~player_handle();

        void frames_in_buffer(int);
    protected:
        SLmillisecond duration();
        SLmillisecond position();
        void enqueue_new_data();

        SLObjectItf m_player;
        SLPlayItf m_play;
        SLSeekItf m_seek;
        SLBufferQueueItf m_queue;
        std::atomic_bool m_playback_over;

        std::vector<int16_t> m_pcm_buffer;
    private:
        void bind_interfaces();
};
