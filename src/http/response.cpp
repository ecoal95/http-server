#include "response.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <socketutils/socketutils.hpp>

#include "runtime_error.hpp"

using socketutils::safe_send;

#define SEND_STATIC(socket, string) safe_send(socket, string, strlen(string), 0);

namespace http {
std::map<int, const char *> response::statuses = {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Moved Temporarily"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Time-out"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Request Entity Too Large"},
	{414, "Request-URI Too Large"},
	{415, "Unsupported Media Type"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Time-out"},
	{505, "HTTP Version not supported"}
};

void response::write_to(int socket, bool close_at_end) {
	std::string body = body_.str();
	SEND_STATIC(socket, "HTTP/1.1 ");
	safe_send(socket, std::to_string(status_).c_str(), 3, 0);
	SEND_STATIC(socket, " ");
	SEND_STATIC(socket, response::statuses[status_]);
	SEND_STATIC(socket, "\r\n");

	headers_["Content-Length"] = std::to_string(body.size()); // +2 because of the las \r\n
	headers_["Connection"] = "close";
	if ( headers_.find("Content-Type") == headers_.end() )
		headers_["Content-Type"] = "text/html";

	for ( auto it = headers_.begin(); it != headers_.end(); ++it ) {
		safe_send(socket, it->first.c_str(), it->first.size(), 0);
		SEND_STATIC(socket, ": ");
		safe_send(socket, it->second.c_str(), it->second.size(), 0);
		SEND_STATIC(socket, "\r\n");
		std::clog << it->first << ": " << it->second << std::endl;
	}
	SEND_STATIC(socket, "\r\n");

	std::clog << body << std::endl;
	safe_send(socket, body.c_str(), body.size(), 0);

	if ( close_at_end )
		close(socket);
}

} // namespace http
