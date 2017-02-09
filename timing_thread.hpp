#ifndef _TIMING_THREAD_HPP
#define _TIMING_THREAD_HPP

#include <thread>
#include <mutex>
#include <condition_variable>


class timing_thread_pool {
public:
    const int nthreads;

    timing_thread_pool(int nthreads);

    typedef struct timeval time_t;

    time_t start_timer();
    double stop_timer(const time_t &start_time);
    
    // Helper function called by timing_thread.
    int get_and_increment_thread_id();

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
    const bool pinned_to_core;
    const int thread_id;
    const int nthreads;

    static void _thread_main(timing_thread *t);

    virtual ~timing_thread() { }

protected:
    timing_thread(const std::shared_ptr<timing_thread_pool> &pool, bool pin_to_core);

    virtual void thread_body() = 0;

    timing_thread_pool::time_t start_time;
    bool timer_is_running = false;

    // Thread-collective: all threads wait at a barrier, then initialize their local timers.
    void start_timer();

    // Thread-collective: the returned time is the average taken over all threads.
    // If 'name' is non-null, then timing will be announced on thread ID zero.
    double stop_timer(const char *name=nullptr);
};


template<typename T, typename... Args>
std::thread spawn_timing_thread(Args... args)
{
    timing_thread *t = new T(args...);
    return std::thread(timing_thread::_thread_main, t);
}


#endif  // _TIMING_THREAD_HPP
