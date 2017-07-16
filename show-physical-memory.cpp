#include <errno.h>
#include <unistd.h>

#include <cmath>
#include <string>
#include <cstring>
#include <iostream>
#include <stdexcept>

using namespace std;


static ssize_t get_physical_memory()
{
    ssize_t pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize < 0)
	throw runtime_error(string("sysconf(_SC_PAGESIZE) failed: ") + strerror(errno));

    ssize_t npages = sysconf(_SC_PHYS_PAGES);
    if (npages < 0)
	throw runtime_error(string("sysconf(_SC_PHYS_PAGES) failed: ") + strerror(errno));

    return npages * pagesize;
}


int main(int argc, char **argv)
{
    ssize_t nbytes = get_physical_memory();

    cout << "nbytes: " << nbytes << endl
	 << "nbytes / 10^9: " << (nbytes/pow(10.,9.)) << endl
	 << "nbytes / 2^30: " << (nbytes/pow(2.,30.)) << endl;

    return 0;
}
