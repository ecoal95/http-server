#ifndef __FILE_UTILS_HPP
#define __FILE_UTILS_HPP

#include <string>
#include <cctype>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace fileutils {

enum file_type {
	none = 0, // unexistant
	socket = S_IFSOCK,
	link = S_IFLNK,
	file = S_IFREG,
	dir = S_IFDIR
};

file_type get_file_type(const char* name);
inline file_type get_file_type(const std::string& s)     { return get_file_type(s.c_str()); }

size_t file_size(const char *name);
inline size_t file_size(const std::string& s)            { return file_size(s.c_str()); }

bool file_exists(const char *name);
inline bool file_exists(const std::string& s)            { return file_exists(s.c_str()); }

void write_file(const char *file_name, int socket);
inline void write_file(const std::string& s, int socket) { return write_file(s.c_str(), socket); }

char *real_path(const char *path);
std::string real_path(const std::string& s);

}
#endif
