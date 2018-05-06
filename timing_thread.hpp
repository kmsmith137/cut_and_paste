#ifndef _TIMING_THREAD_HPP
#define _TIMING_THREAD_HPP

#include <thread>
#include <mutex>
#include <condition_variable>


class timing_thread_pool {
public:
    const int nthreads;

    timing_thread_pool(int nthreads);
    
    // Helper function called by timing_thread.
    int get_and_increment_thread_id();

    // A "reducing" barrier.  When each thread arrives at the barrier, it
    // specifies a value of "t".  The return value from wait_at_barrier()
    // is the mean (over all threads) of the t-values.
    double wait_at_barrier(double t=0);

protected:
    // Assigning thread ID's.
    std::mutex thread_id_lock;
    int curr_thread_id = 0;
    
    // Barrier.
    std::mutex barrier_lock;
    std::condition_variable barrier_cv;
    double barrier_tcurr = 0.0;
    double barrier_tprev = 0.0;
    int barrier_count = 0;
    int barrier_gen = 0;
};


class timing_thread {
public:
    const std::shared_ptr<timing_thread_pool> pool;
    const bool pinned_to_core;
    const bool call_warm_up_cpu;
    const int thread_id;
    const int nthreads;

    // If 'name' is a nonempty string, then timings will be printed on thread ID zero 
    // when stop_timer() is called (see below).  If 'nbytes_accessed' and 'floating_point_ops'
    // are nonzero, then memory bandwidth and flops will also be printed.

    std::string name;
    ssize_t nbytes_accessed = 0;
    ssize_t floating_point_ops = 0;

    static void _thread_main(timing_thread *t);

    virtual ~timing_thread() { }

protected:
    timing_thread(const std::shared_ptr<timing_thread_pool> &pool, bool pin_to_core, bool warm_up_cpu=true);

    virtual void thread_body() = 0;

    struct timeval start_time;
    bool timer_is_running = false;
    
    double local_dt = 0.0;
    double global_dt = 0.0;
    
    // Thread-collective: all threads wait at a barrier, then start their local timers.
    void start_timer();
    
    // Thread-collective: snychronizes and computes 'global_dt', the average running time on all threads.
    // If 'name' is non-null, then timing will be announced on thread ID zero.
    void stop_timer();

    // Temporarily pause local timer, if there is some processing which should
    // be excluded from the timing.  (Not thread-collective.)
    void pause_timer();
    void unpause_timer();
};


template<typename T, typename... Args>
std::thread spawn_timing_thread(Args... args)
{
    timing_thread *t = new T(args...);
    return std::thread(timing_thread::_thread_main, t);
}


#endif  // _TIMING_THREAD_HPP
