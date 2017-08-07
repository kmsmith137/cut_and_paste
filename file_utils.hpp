#include <vector>
#include <string>

#ifndef _FILE_UTILS_HPP
#define _FILE_UTILS_HPP


extern bool file_exists(const std::string &filename);
extern bool is_directory(const std::string &filename);
extern bool is_empty_directory(const std::string &filename);

extern std::vector<std::string> listdir(const std::string &dirname);

// Note: may return file descriptors which have already been closed.
extern std::vector<int> get_open_file_descriptors();

extern void close_all_file_descriptors(int min_fd);


#endif  // _FILE_UTILS_HPP
