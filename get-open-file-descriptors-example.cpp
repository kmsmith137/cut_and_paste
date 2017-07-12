#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "file_utils.hpp"

using namespace std;


static int noisy_open()
{
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0)
	throw runtime_error(string("open('/dev/null') failed: ") + strerror(errno));
    cout << "opened /dev/null: fd=" << fd << endl;
    return fd;
}


static void noisy_close(int fd)
{
    int err = close(fd);
    if (err < 0)
	throw runtime_error("close(fd=" + to_string(fd) + ") failed: " + strerror(errno));
    cout << "closed fd=" << fd << endl;
}


int main(int argc, char **argv)
{
    int fd1 = noisy_open();
    int fd2 = noisy_open();
    int fd3 = noisy_open();
    int fd4 = noisy_open();

    noisy_close(fd2);
    
    vector<int> open_fds = get_open_file_descriptors();
    std::sort(open_fds.begin(), open_fds.end());
    
    cout << "get_open_file_descriptors() returned: [";
    for (unsigned int i = 0; i < open_fds.size(); i++)
	cout << " " << open_fds[i];
    cout << " ]\n";
    
    return 0;
}
