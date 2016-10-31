#include <vector>
#include <iostream>
#include <unistd.h>
#include "timing_thread.hpp"

using namespace std;


class my_timing_thread : public timing_thread {
public:
    my_timing_thread(const shared_ptr<timing_thread_pool> &pool_) :
	timing_thread(pool_, true)   // pin_to_core=true
    { }

    void time_sleep()
    {
	this->start_timer();
	sleep(5 + this->thread_id);
	double dt = this->stop_timer();

	if (thread_id == 0)
	    cerr << "time_sleep: " << dt << endl;
    }

    virtual void thread_body() override
    {
	this->time_sleep();
	this->time_sleep();
    }
};


int main(int argc, char **argv)
{
    const int nthreads = 4;
    auto pool = make_shared<timing_thread_pool> (nthreads);

    vector<thread> threads(nthreads);
    for (int i = 0; i < nthreads; i++)
	threads[i] = spawn_timing_thread<my_timing_thread> (pool);
    for (int i = 0; i < nthreads; i++)
	threads[i].join();

    return 0;
}
