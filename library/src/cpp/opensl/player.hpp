#pragma once
#include "context.hpp"

namespace opensl {
    class player {
        public:
            player(const context& p_context, SLDataSource p_source, SLDataSink p_sink);
            ~player();
        private:
            SLObjectItf m_player;
            SLPlayItf m_play;
            SLSeekItf m_seek;
            SLBufferQueueItf m_queue;
    };
}