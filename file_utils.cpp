#include <errno.h>
#include <fcntl.h>
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


bool is_directory(const string &filename)
{
    struct stat s;

    int err = stat(filename.c_str(), &s);
    if (err < 0)
	throw runtime_error(filename + ": " + strerror(errno));

    return S_ISDIR(s.st_mode);
}


bool is_empty_directory(const string &dirname)
{
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
	    return true;
	if (!strcmp(entry->d_name, "."))
	    continue;
	if (!strcmp(entry->d_name, ".."))
	    continue;
	
	return false;
    }
}


void makedir(const string &filename, bool throw_exception_if_directory_exists, mode_t mode)
{
    int err = mkdir(filename.c_str(), mode);

    if (err >= 0)
	return;
    if (throw_exception_if_directory_exists || (errno != EEXIST))
	throw runtime_error(filename + ": mkdir() failed: " + strerror(errno));
    
    // If we get here, then mkdir() failed with EEXIST, and throw_exception_if_directory_exists=false.
    // We still throw an exception if the file is not a directory.

    struct stat s;
    err = stat(filename.c_str(), &s);

    // A weird corner case.
    if (err < 0)
	throw runtime_error(filename + ": mkdir() returned EEXIST but stat() failed, not sure what is going on");

    if (!S_ISDIR(s.st_mode))
	throw runtime_error(filename + ": file exists but is not a directory");
}


ssize_t get_file_size(const string &filename)
{
    struct stat s;

    int err = stat(filename.c_str(), &s);
    if (err < 0)
	throw runtime_error(filename + ": " + strerror(errno));

    return s.st_size;
}


void delete_file(const string &filename)
{
    int err = unlink(filename.c_str());
    if (err < 0)
	throw runtime_error(filename + ": unlink() failed: " + strerror(errno));
}


void sync_filesystem(const string &filename)
{
    // Linux has the syncfs() system call, which syncs the filesystem
    // containing a given file descriptor.  Otherwise the best we can
    // do is sync(), which syncs all filesystems!
    
#if defined(__linux__)
    int fd = open(filename.c_str(), O_RDONLY, 0);
    if (fd < 0)
	throw runtime_error(filename + ": open() failed: " + strerror(errno));
    
    int err = syncfs(fd);
    close(fd);
    
    if (err < 0)
	throw runtime_error(filename + ": syncfs() failed: " + strerror(errno));
#else
    sync();
#endif
}


void write_file(const string &filename, const void *buf, ssize_t count, bool clobber)
{
    // This cast to (uint8_t *) suppresses a superfluous compiler warning below.
    uint8_t *p = (uint8_t *) buf;
    
    if (count < 0)
	throw runtime_error("write_file(): expected count >= 0");
    if (count && !p)
	throw runtime_error("write_file(): 'buf' is a null pointer");
	
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    if (!clobber)
	flags |= O_EXCL;

    // Reasonable default?  owner=rw, group=r, other=r (note that umask will also be applied)
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int fd = open(filename.c_str(), flags, mode);
    if (fd < 0)
	throw runtime_error(filename + ": open() failed: " + strerror(errno));

    // We loop over multiple calls to write() because, according to the manpage,
    // write() is allowed to write a subset of the buffer and return the number of
    // bytes actually written.  I've never seen this actually happen though!

    while (count > 0) {
	ssize_t n = write(fd, p, count);

	if (n <= 0) {
	    close(fd);
	    const char *msg = (n < 0) ? strerror(errno) : "write() returned 0?!";
	    throw runtime_error(filename + ": write() failed: " + msg);
	}
	
	count -= n;
	p += n;
    }

    close(fd);
}


void hard_link(const string &src_filename, const string &dst_filename)
{
    int err = link(src_filename.c_str(), dst_filename.c_str());
    if (err < 0)
	throw runtime_error("hard_link() failed (" + src_filename + " -> " + dst_filename + ": " + strerror(errno));
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
	if ((s == ".") || (s == ".."))
	    continue;
	
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


// -------------------------------------------------------------------------------------------------


void close_all_file_descriptors(int min_fd)
{
    for (int fd: get_open_file_descriptors()) {
	// Don't check return value from close(), since get_open_file_descriptors()
	// returns previously closed fds.
	if (fd >= min_fd)
	    close(fd);
    }
}
