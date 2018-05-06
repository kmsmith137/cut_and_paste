#include <cmath>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "timing_thread.hpp"

using namespace std;


static void pin_current_thread_to_core(int core_id)
{
#ifdef __APPLE__
    if (core_id == 0)
	cerr << "warning: pinning threads to cores is not implemented in osx\n";
    return;
#else
    int hwcores = std::thread::hardware_concurrency();
    
    if ((core_id < 0) || (core_id >= hwcores))
	throw runtime_error("pin_thread_to_core: core_id=" + to_string(core_id) + " is out of range (hwcores=" + to_string(hwcores) + ")");

    pthread_t thread = pthread_self();

    cpu_set_t cs;
    CPU_ZERO(&cs);
    CPU_SET(core_id, &cs);

    int err = pthread_setaffinity_np(thread, sizeof(cs), &cs);
    if (err)
        throw runtime_error("pthread_setaffinity_np() failed");
#endif
}


void warm_up_cpu()
{
    // A throwaway computation which uses the CPU for ~10^9
    // clock cycles.  The details (usleep, xor) are to prevent the
    // compiler from optimizing it out!
    //
    // Empirically, this makes timing results more stable (without it,
    // the CPU seems to run slow for the first ~10^9 cycles or so.)

    long n = 0;
    for (long i = 0; i < 1000L * 1000L * 1000L; i++)
	n += (i ^ (i-1));
    usleep(n % 2);
}


// -------------------------------------------------------------------------------------------------
//
// timing_thread_pool


timing_thread_pool::timing_thread_pool(int nthreads_) :
    nthreads(nthreads_)
{ 
    if (nthreads <= 0)
	throw runtime_error("timing_thread_pool constructor called with nthreads <= 0");
}


int timing_thread_pool::get_and_increment_thread_id()
{
    lock_guard<mutex> l(thread_id_lock);
    return curr_thread_id++;
}


double timing_thread_pool::wait_at_barrier(double t)
{
    unique_lock<mutex> l(barrier_lock);

    barrier_tcurr += t;
    barrier_count++;
    
    if (barrier_count == nthreads) {
	barrier_tprev = barrier_tcurr;
	barrier_tcurr = 0.0;
	barrier_count = 0;
	barrier_gen++;
	barrier_cv.notify_all();
	return barrier_tprev / nthreads;
    }
    
    int g = barrier_gen;

    while (barrier_gen == g)
	barrier_cv.wait(l);

    return barrier_tprev / nthreads;
}


// -------------------------------------------------------------------------------------------------
//
// timing_thread


timing_thread::timing_thread(const shared_ptr<timing_thread_pool> &pool_, bool pin_to_core, bool warm_up_cpu_) :
    pool(pool_), 
    pinned_to_core(pin_to_core),
    call_warm_up_cpu(warm_up_cpu_),
    thread_id(pool_->get_and_increment_thread_id()),
    nthreads(pool_->nthreads)
{ }


// static member function
void timing_thread::_thread_main(timing_thread *t)
{
    // Ensure delete(t) is called
    auto p = unique_ptr<timing_thread> (t);

    if (t->pinned_to_core)
	pin_current_thread_to_core(t->thread_id);

    // Call after pinning thread
    if (t->call_warm_up_cpu)
	warm_up_cpu();

    t->thread_body();
}


void timing_thread::start_timer()
{
    pool->wait_at_barrier();
    
    this->local_dt = 0.0;
    this->unpause_timer();
}


void timing_thread::stop_timer()
{
    this->pause_timer();
    this->global_dt = pool->wait_at_barrier(local_dt);

    if ((thread_id != 0) || (name.size() == 0))
	return;

    cout << name << ": " << global_dt << " seconds";

    if (nbytes_accessed > 0)
	cout << ", memory bandwidth " << (nbytes_accessed / global_dt / pow(2.,30.)) << " GB/sec";
    if (floating_point_ops > 0)
	cout << ", gflops=" << (floating_point_ops / global_dt / pow(2.,30.));

    cout << endl;
}


void timing_thread::pause_timer()
{
    if (!timer_is_running)
	throw runtime_error("timing_thread::stop_timer() or pause_timer() was called, but timer was already stopped");

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    // Note "+=" here.
    this->local_dt += (end_time.tv_sec - start_time.tv_sec) + 1.0e-6 * (end_time.tv_usec - start_time.tv_usec);
    this->timer_is_running = false;
}

void timing_thread::unpause_timer()
{
    if (timer_is_running)
	throw runtime_error("timing_thread::start_timer() or unpause_timer() was called, but timer is already running");
    
    gettimeofday(&this->start_time, NULL);
    this->timer_is_running = true;
}
