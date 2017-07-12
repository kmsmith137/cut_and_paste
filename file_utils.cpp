#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include <cstring>
#include <sstream>

#include "file_utils.hpp"
#include "lexical_cast.hpp"

using namespace std;


bool file_exists(const string &filename)
{
    struct stat s;

    int err = stat(filename.c_str(), &s);
    if (err >= 0)
        return true;
    if (errno == ENOENT)
        return false;

    throw runtime_error(filename + ": " + strerror(errno));
}


vector<string> listdir(const string &dirname)
{
    vector<string> filenames;

    DIR *dir = opendir(dirname.c_str());
    if (!dir)
	throw runtime_error(dirname + ": opendir() failed: " + strerror(errno));

    ssize_t name_max = pathconf(dirname.c_str(), _PC_NAME_MAX);
    name_max = min(name_max, (ssize_t)4096);

    vector<char> buf(sizeof(struct dirent) + name_max + 1);
    struct dirent *entry = reinterpret_cast<struct dirent *> (&buf[0]);
    
    for (;;) {
	struct dirent *result = nullptr;

	int err = readdir_r(dir, entry, &result);	
	if (err)
	    throw runtime_error(dirname + ": readdir_r() failed");
	if (!result)
	    break;

	filenames.push_back(entry->d_name);
    }

    return filenames;
}


// -------------------------------------------------------------------------------------------------
//
// get_open_file_descriptors()


static vector<int> _get_open_fds(const char *fd_dirname)
{
    vector<int> ret;

    for (const string &s: listdir(fd_dirname)) {
	int fd;

	if (lexical_cast(s, fd)) {
	    ret.push_back(fd);
	    continue;
	}

	stringstream ss;
	ss << "get_open_file_descriptors(): filename '" << fd_dirname << "/" << s << "' is not an integer";
	throw runtime_error(ss.str());
    }

    return ret;
}

#if defined(__linux__)
vector<int> get_open_file_descriptors() { return _get_open_fds("/proc/self/fd"); }
#elif defined(__APPLE__)
vector<int> get_open_file_descriptors() { return _get_open_fds("/dev/fd"); }
#else
vector<int> get_open_file_descriptors() { throw runtime_error("get_open_file_descriptors() is only implemented on linux and osx"); }
#endif
