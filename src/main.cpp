#include "http/server.hpp"
#include <iostream>

void callback(const http::request& request, http::response& response) {
	response.status(404);

	response << "<!doctype html>"
		<< "<html><title>hello</title>"
		<< "<body><p>";
	if ( request.path == "/" )
		response << "Hello there!";
	else
		response << "Hello " << request.path.substr(1);
}


int main(int argc, const char **argv) {
	http::server s;
	int port = 8080;

	if ( argc > 1 )
		port = atoi(argv[1]);

	s.listen(port).receive(callback);
	return 0;
}
