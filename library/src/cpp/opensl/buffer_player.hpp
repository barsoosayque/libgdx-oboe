#pragma once
#include <functional>
#include <vector>
#include <atomic>
#include "context.hpp"

namespace opensl {
    class buffer_player {
        public:
            using buffer_callback = std::function<void(const std::vector<int16_t>&)>;

            buffer_player(const context& p_context, SLDataSource p_source);
            ~buffer_player();

            void on_buffer_update(buffer_callback p_callback);

            void enqueue();
            void resize_buffer(int p_size);

            void play();
            void pause();
            void stop();

            bool is_working();
        private:
            buffer_callback m_buffer_callback;
            std::vector<int16_t> m_queue_buffer;
            std::atomic_int m_queued_buffers;
            std::atomic_bool m_playback_over;

            SLObjectItf m_player;
            SLPlayItf m_play;
            SLSeekItf m_seek;
            SLBufferQueueItf m_queue;
    };
}