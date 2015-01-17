#ifndef __HTTP_UTILS_HPP
#define __HTTP_UTILS_HPP
#include "server.hpp"
#include <vector>
#include <string>
#include <map>

namespace http {
namespace utils {
std::map<std::string, std::string> process_headers(std::vector<std::string>& headers);
void parse_request_first_line(http::request& req, std::string& line);

/* Fast simple string hash (Bernstein?) */
constexpr unsigned int const_hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (const_hash(s, off+1)*33) ^ s[off];
}

bool file_exists(const char *name);

inline bool file_exists(const std::string& name) {
	return file_exists(name.c_str());
}

void write_file(const char *file_name, int socket);

} // namespace utils
} // namespace http

#endif
