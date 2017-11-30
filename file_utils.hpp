#include <vector>
#include <string>
#include <sys/stat.h>

#ifndef _FILE_UTILS_HPP
#define _FILE_UTILS_HPP


extern bool file_exists(const std::string &filename);
extern bool is_directory(const std::string &filename);
extern bool is_empty_directory(const std::string &filename);

extern void delete_file(const std::string &filename);
extern void write_file(const std::string &filename, const void *buf, ssize_t count, bool clobber);
extern void hard_link(const std::string &src_filename, const std::string &dst_filename);
extern void sync_filesystem(const std::string &filename);
extern ssize_t get_file_size(const std::string &filename);

// Note: umask will be applied to 'mode'
extern void makedir(const std::string &filename, bool throw_exception_if_directory_exists=true, mode_t mode=0777);
extern std::vector<std::string> listdir(const std::string &dirname);

// Note: may return file descriptors which have already been closed.
extern std::vector<int> get_open_file_descriptors();

extern void close_all_file_descriptors(int min_fd);


#endif  // _FILE_UTILS_HPP
