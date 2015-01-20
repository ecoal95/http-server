#include "fileutils.hpp"
#include <socketutils/socketutils.hpp>

#include <iostream>
#include <cstdlib>

#include <cstring>
namespace fileutils {

file_type get_file_type(const char *name) {
	struct stat s;
	if ( stat(name, &s) != 0 )
		return file_type::none;

	return static_cast<file_type>(s.st_mode);
}

size_t file_size(const char *name) {
	struct stat s;
	
	if ( stat(name, &s) != 0 )
		return (size_t) -1;

	return s.st_size;
}

bool file_exists(const char *name) {
	struct stat s;

	if ( stat(name, &s) != 0 )
		return false;

	return s.st_mode & S_IFREG;
}


#define RESPONSE_STATIC_CHUNK_SIZE 1024

void write_file(const char *file_name, int socket) {
	char buffer[RESPONSE_STATIC_CHUNK_SIZE];
	size_t read;
	FILE *file = fopen(file_name, "r");

	if ( file == NULL )
		return;

	while ( (read = fread(buffer, 1, RESPONSE_STATIC_CHUNK_SIZE, file)) ) {
		std::clog << "Read " << read << " bytes." << std::endl;
		socketutils::safe_send(socket, buffer, read, 0);
	}
}

/** Get the real path */
char *real_path(const char *path) {
	char *ret;

	if ( (ret = ::realpath(path, NULL)) == NULL ) {
		std::clog << "Couldn't determine real path for " << path << std::endl;
		return strdup(path);
	}

	return ret;
}

std::string real_path(const std::string& s) {
	char *r = real_path(s.c_str());
	std::string real(r);
	delete[] r;
	return real;
}

} // namespace fileutils
