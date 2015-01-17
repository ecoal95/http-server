#ifndef __HTTP_REQUEST_HPP
#define __HTTP_REQUEST_HPP
#include <map>
#include <string>
namespace http {
struct request {
	std::string method;
	std::string path;
	std::string protocol;
	std::map<std::string, std::string> headers;
	std::string body;

	request(int socket) : socket_(socket) { parse(); };
private:
	void parse(); // Fill request body with contents from socket
	int socket_;
};

} // namespace http

#endif
