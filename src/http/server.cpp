#include "server.hpp"
#include "utils.hpp"
#include <thread>
#include <iostream>
#include <unistd.h> // close
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

	std::clog << "Server <" << this << "> is now listening to " << htonl(INADDR_LOOPBACK) << ":" << port << std::endl;

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
		std::thread(&server::process_request, this, new_socket, cb).detach();
	}
}


void server::process_request(int socket, callback_type cb) {
	std::clog << "Connection stabilished (socket id: " << socket << ")" << std::endl;

	try {

	request req(socket);
	response resp;

	if ( req.protocol != "HTTP/1.1" && req.protocol != "HTTP/1.0" )
		throw runtime_error("Malformed request: Invalid protocol");

	switch ( utils::const_hash(req.method.c_str()) ) {
		case utils::const_hash("GET"):
		case utils::const_hash("HEAD"):
			std::cout << "GET or HEAD request!" << std::endl;
			break;
		case utils::const_hash("POST"):
		case utils::const_hash("OPTIONS"):
		case utils::const_hash("PUT"):
		case utils::const_hash("PATCH"):
		case utils::const_hash("DELETE"):
			std::cout << "not implemented yet :/" << std::endl;
		default:
			throw runtime_error("Malformed request: Invalid protocol");
	}

	std::cout << "Body: " << req.body << std::endl;


	/** TODO: This actually searchs from the root... WTF?? */
	std::string file_name = root_dir_ + req.path;

	std::clog << "Trying " << file_name << std::endl;

	if ( utils::file_exists(file_name) )
		return serve_file(resp, file_name, socket);

	std::cout << req.method << " " << req.path << " " << req.protocol << std::endl;
	auto headers = req.headers;
	for ( auto it = headers.begin(); it != headers.end(); ++it )
		std::cout << it->first << ":" << it->second << std::endl;

	cb(req, resp);

	resp.write_to(socket);

	} catch ( runtime_error e ) {
		throw;
	}
}

void server::serve_file(response& resp, std::string& file_name, int socket) {
	std::clog << "Serving file: " << file_name << std::endl;
	resp.header("Content-Type", "text/plain"); // TODO
	resp.write_to(socket, false); // don't close (yet)
	utils::write_file(file_name.c_str(), socket);
	close(socket);
}

} // namespace http
