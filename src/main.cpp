#include "http/server.hpp"
#include <iostream>

void callback(void) {
	std::cout << "Received connection!" << std::endl;
}


int main(int argc, const char **argv) {
	http::server s;
	int port = 8080;

	if ( argc > 1 )
		port = atoi(argv[1]);

	s.listen(port).receive(callback);
	return 0;
}
