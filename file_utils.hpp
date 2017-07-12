#include <vector>
#include <string>

#ifndef _FILE_UTILS_HPP
#define _FILE_UTILS_HPP

// More to come!

extern bool file_exists(const std::string &filename);

extern std::vector<std::string> listdir(const std::string &dirname);

extern std::vector<int> get_open_file_descriptors();


#endif  // _FILE_UTILS_HPP
