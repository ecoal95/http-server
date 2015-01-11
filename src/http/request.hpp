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
};

} // namespace http

#endif
