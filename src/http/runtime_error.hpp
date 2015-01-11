#ifndef __HTTP_RUNTIME_ERROR_HPP
#define __HTTP_RUNTIME_ERROR_HPP

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace http {

struct runtime_error : public std::runtime_error {
public:
	explicit runtime_error(const char *what_msg) : std::runtime_error(what_msg) {};
	explicit runtime_error() : std::runtime_error(strerror(errno)) {};
};

}

#endif
