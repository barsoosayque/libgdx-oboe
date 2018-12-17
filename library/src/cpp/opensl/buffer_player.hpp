#pragma once
#include <functional>
#include <vector>
#include <atomic>
#include "context.hpp"

namespace opensl {
    class buffer_player {
        public:
            using pcm_container = std::vector<int16_t>;
            using buffer_iterator = pcm_container::const_iterator;
            using buffer_callback = std::function<void(buffer_iterator, buffer_iterator)>;

            buffer_player(const context& p_context, SLDataSource p_source);
            ~buffer_player();

            void on_buffer_update(buffer_callback p_callback);

            // pass -1 to enqueue to the EOF
            void enqueue(int p_buffers);
            void resize_buffer(int p_size);
            int buffer_size();

            void play();
            void pause();
            void stop();

            float duration() const;
            float position() const;

            // in seconds
            void seek(float p_position);

            bool is_working() const;

            float samples_to_seconds(int p_samples);
        private:
            void enqueue();

            buffer_callback m_buffer_callback;
            pcm_container m_queue_buffer;
            std::atomic_int m_queued_buffers;
            std::atomic_bool m_playback_over;

            SLObjectItf m_player;
            SLPlayItf m_play;
            SLSeekItf m_seek;
            SLBufferQueueItf m_queue;

            SLmillisecond m_duration;
            mutable SLmillisecond m_position;
    };
}