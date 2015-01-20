#ifndef __HTTP_SERVER_HPP
#define __HTTP_SERVER_HPP

#include <socketutils/socketutils.hpp>
#include "runtime_error.hpp"
#include "request.hpp"
#include "response.hpp"
#define DEFAULT_MAX_CONNECTIONS 1024

namespace http {

class server {
	int socket_;
	std::string root_dir_;
	uint max_connections_;
public:
	typedef uint port_type;
	typedef void (*callback_type)(const request&, response&);
	server(const std::string&, uint max_connections) throw(runtime_error);
	server(const std::string& s) : server(s, DEFAULT_MAX_CONNECTIONS) {};
	server() : server(".") {};

	server& listen(port_type);
	void receive(callback_type cb);
private:
	static void serve_file(response& resp, std::string& file_name, int socket);
	void process_request(int new_socket, callback_type cb);
};

} // namespace http

#endif
