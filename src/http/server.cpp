#include "server.hpp"
#include "utils.hpp"
#include <thread>
#include <iostream>
namespace http {

server::server(const std::string& root_dir, uint max_connections) throw(runtime_error)
: root_dir_(root_dir), max_connections_(max_connections) {
	socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if ( socket_ == -1 )
		throw runtime_error();

	std::clog << "Started server <" << this << ">:" << std::endl <<
		"\tsocket id: " << socket_ << std::endl <<
		"\troot directory: " << root_dir_ << std::endl;
}

server& server::listen(port_type port) {
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(port);

	if ( bind(socket_, (struct sockaddr *) &addr, sizeof(addr)) == -1 )
		throw runtime_error();

	if ( ::listen(socket_, max_connections_) == -1 )
		throw runtime_error();

	std::clog << "Server <" << this << "> is now listening to " << INADDR_LOOPBACK << ":" << port << std::endl;

	return *this;
}

void server::receive(callback_type cb) {
	int new_socket;
	struct sockaddr_in client;
	socklen_t client_size = sizeof(client);
	while ( true ) {
		new_socket = accept(socket_, (struct sockaddr*) &client, &client_size);
		if ( new_socket == -1 )
			throw runtime_error();
		std::thread(utils::process_request, new_socket, cb).detach();
	}
}

} // namespace http
