#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

class executor {
    public:
        executor(std::function<void()> job)
            : m_run(true)
            , m_done(false)
            , m_job(std::move(job))
            , m_worker(std::bind(&executor::run, this))
        {}

        ~executor() {
            m_run.clear();
            m_cond.notify_all();
            m_worker.join();
        }

        void queue() {
            m_done.store(false);
            m_cond.notify_all();
        }

        void wait() {
            while(!m_done.load()) {
                m_cond.notify_all();
            }
        }
    private:
        void run() {
            while(m_run.test_and_set()) {
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_cond.wait(lock);
                }

                if(!m_done.load()) {
                    m_job();
                    m_done.store(true);
                }
            }
        }

        std::atomic_flag m_run;
        std::atomic_bool m_done;
        std::function<void()> m_job;
        std::thread m_worker;
        std::mutex  m_mutex;
        std::condition_variable m_cond;
};
