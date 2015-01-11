#ifndef __HTTP_SERVER_HPP
#define __HTTP_SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "runtime_error.hpp"
#include "request.hpp"
#include "response.hpp"
#define DEFAULT_MAX_CONNECTIONS 1024

namespace http {

class server {
	int socket_;
	const std::string& root_dir_;
	uint max_connections_;
public:
	typedef uint port_type;
	typedef void (*callback_type)(const request&, response&);
	server(const std::string&, uint max_connections) throw(runtime_error);
	server(const std::string& s) : server(s, DEFAULT_MAX_CONNECTIONS) {};
	server() : server(".") {};

	server& listen(port_type);
	void receive(callback_type cb);
};

} // namespace http

#endif
