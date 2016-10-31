#ifndef _TIMING_THREAD_HPP
#define _TIMING_THREAD_HPP

#include <thread>
#include <mutex>
#include <condition_variable>


class timing_thread_pool {
public:
    const int nthreads;

    timing_thread_pool(int nthreads);

    int get_ithread();

    struct timeval start_timer();
    double stop_timer(const struct timeval &tv0);

protected:
    std::mutex lock;
    std::condition_variable cond0;
    std::condition_variable cond1;
    std::condition_variable cond2;

    double total_dt = 0.0;
    int threads_so_far = 0;

    int ix0 = 0;
    int ix1 = 0;
    int ix2 = 0;
};


class timing_thread {
public:
    const std::shared_ptr<timing_thread_pool> pool;
    const int thread_id;
    const bool pinned_to_core;

    static void _spawn(timing_thread *t);

protected:
    timing_thread(const std::shared_ptr<timing_thread_pool> &pool, bool pin_to_core);

    virtual void thread_body() = 0;

    struct timeval tv0;
    bool timer_is_running = false;

    void start_timer();
    double stop_timer();
};


template<typename T, typename... Args>
std::thread spawn_timing_thread(Args... args)
{
    timing_thread *t = new T(args...);
    return std::thread(timing_thread::_spawn, t);
}


#endif  // _TIMING_THREAD_HPP
