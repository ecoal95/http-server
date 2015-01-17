#ifndef __SOCKET_UTILS_HPP
#define __SOCKET_UTILS_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

namespace socketutils {

inline size_t send(int socket, const char *buffer, size_t size, int flags = 0) {
	return ::send(socket, buffer, size, flags);
}

void safe_send(int socket, const char *buffer, size_t size, int flags = 0);

inline void safe_send(int socket, const std::string& buffer, size_t size, int flags = 0) {
	return safe_send(socket, buffer.c_str(), size, flags);
}


}
#endif
