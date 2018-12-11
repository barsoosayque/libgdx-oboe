#pragma once
#include "context.hpp"
#include <map>

namespace opensl {
    class player {
        public:
            using features = std::map<SLInterfaceID, SLboolean>;

            player(const context& p_context, SLDataSource p_source, SLDataSink p_sink, const features& p_features);
            ~player();
        private:
            SLObjectItf m_player;
            SLPlayItf m_play;
            SLSeekItf m_seek;
            SLBufferQueueItf m_queue;
    };
}