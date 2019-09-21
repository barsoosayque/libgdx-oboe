#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>

class executor {
    public:
        executor(std::function<void()> job)
            : m_is_shutdown(false)
            , m_queue_size(0)
            , m_thread(std::bind(&executor::job_watcher, this, job))
        {}
        ~executor() {
            stop();
            m_is_shutdown = true;
        }

        void queue(int count = 1) {
            std::scoped_lock<std::recursive_mutex> l(m_mutex);
            m_queue_size += count;
        }
        void wait() {
            std::scoped_lock<std::recursive_mutex> l(m_mutex);
        }
        void stop() {
            std::scoped_lock<std::recursive_mutex> l(m_mutex);
            m_queue_size = 0;
            wait();
        }
    private:
        void job_watcher(std::function<void()> job) {
            while(!m_is_shutdown) {
                std::scoped_lock<std::recursive_mutex> l(m_mutex);
                if(m_queue_size > 0) {
                    job();
                    --m_queue_size;
                }
            }
        }

        bool m_is_shutdown;
        int m_queue_size;
        std::recursive_mutex m_mutex;
        std::thread m_thread;
};
