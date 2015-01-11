#include "utils.hpp"
#include "request.hpp"
#include "response.hpp"
#include <iostream>
#include <vector>
#include <map>

#define REQUEST_HEADER_LINE_MAX_SIZE 1024

namespace http {
namespace utils {

/* Fast simple string hash (Bernstein?) */
constexpr unsigned int const_hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (const_hash(s, off+1)*33) ^ s[off];
}

/** Trim a string */
std::string& trim(std::string& str) {
	size_t index = str.find_first_not_of(' ');

	if ( index != std::string::npos )
		str.substr(index).swap(str);

	index = str.find_last_not_of(' ');

	if ( index != std::string::npos )
		str.substr(0, index + 1).swap(str);

	return str;
}

/** Get a map of headers given a vector: split and trim */
std::map<std::string, std::string> process_headers(std::vector<std::string>& headers) {
	std::map<std::string, std::string> ret;

	for ( auto it = headers.begin(); it != headers.end(); ++it ) {
		auto pos = (*it).find(':');
		std::string key = it->substr(0, pos);
		std::string val = it->substr(pos + 1);
		ret[trim(key)] = trim(val);
	}

	return ret;
}

/** Parse the request first line: method, url and protocol */
void parse_request_first_line(http::request& req, std::string& line) {
	std::string::size_type size = line.size();
	char *original_str = new char[size + 1];
	char *str = original_str;
	char *tmp = str;

	strncpy(str, line.c_str(), size);
	str[size] = '\0';

	// discard initial whitespace
	while ( *str == ' ' ) str++;

	// method: until we find one space
	while ( *str && *str++ != ' ' );
	if ( *(str - 1) == '\0' ) {  // We reached the end of the line without a space
		delete[] original_str;
		throw runtime_error("Malformed request");
	}
	*(str - 1) = '\0';
	req.method = tmp;

	// discard spaces
	while ( *str == ' ' ) str++;
	tmp = str;

	while ( *str && *str++ != ' ' );
	if ( *(str - 1) == '\0' ) {  // We reached the end of the line without a space
		delete[] original_str;
		throw runtime_error("Malformed request");
	}
	*(str - 1) = '\0';
	req.path = tmp;

	// discard spaces
	while ( *str == ' ' ) str++;
	tmp = str;
	while ( *str && *str++ != ' ' ); // now we expect the end of line
	req.protocol = tmp;

	delete[] original_str;
}


/** Parse the whole request */
request get_request(int socket) {
	size_t ret;

	char buff[REQUEST_HEADER_LINE_MAX_SIZE];
	char *str = NULL, *tmp;
	char c;

	bool end = false;
	bool check_next_newline = false;
	bool last_scan_header_end = true;

	request req;
	std::string request_type;
	std::vector<std::string> headers = {""};
	std::string body;

	while ( !end && (ret = recv(socket, buff, REQUEST_HEADER_LINE_MAX_SIZE - 1, 0)) > 0 ) {
		buff[ret] = '\0';
		tmp = str = buff;

		if ( check_next_newline ) { // if last header end could got chunked
			if ( *str == '\n' ) { // and really was a header end
				str++;
				tmp++;
				headers.push_back("");
			} else { // append last character
				headers.back() += '\r';
				last_scan_header_end = false;
			}
		}

		while ( (c = *str++) ) {
			// found CLRF -> put what whe have
			if ( c == '\r' && *str++ == '\n' ) {
				*(str - 2) = '\0';

				// if we had a second clrf, it means that we have reached the end of the headers
				if ( last_scan_header_end && (str - 2) == tmp ) {
					end = true;
					break;
				} else {
					headers.back() += tmp;
					headers.push_back("");
					// We scanned successfully a header
					last_scan_header_end = true;
					tmp = str; // and keep searching
				}
			}
		}


		if ( ! end ) {
			if ( *(str - 2) == '\r' ) { // if last char of chunk is \r...
				// say we only need to check for '\n' and remove unsafe part
				check_next_newline = true;
				*(str - 2) = '\0';
			}

			last_scan_header_end = (*tmp == '\0');

			headers.back() += tmp;
		}
	}

	// Remove empty header
	if ( headers.back().empty() )
		headers.pop_back();

	parse_request_first_line(req, headers.front());

	headers.erase(headers.begin());

	req.headers = process_headers(headers);

	req.body = str - 1;

	return req;
}

void process_request(int socket, server::callback_type cb){
	std::clog << "Connection stabilished (socket id: " << socket << ")" << std::endl;
	request req;
	response resp;
	try {
		req = get_request(socket);
	} catch ( runtime_error e ) {
		// throw malformed request
		return;
	}

	if ( req.protocol != "HTTP/1.1" && req.protocol != "HTTP/1.0" ) {
		// throw malformed request: invalid protocol
		return;
	}

	switch ( const_hash(req.method.c_str()) ) {
		case const_hash("GET"):
		case const_hash("HEAD"):
			std::cout << "GET or HEAD request!" << std::endl;
			break;
		case const_hash("POST"):
		case const_hash("OPTIONS"):
		case const_hash("PUT"):
		case const_hash("PATCH"):
		case const_hash("DELETE"):
			std::cout << "not implemented yet :/" << std::endl;
		default:
			// throw malformed request: invalid method
			return;
	}

	std::cout << "Body: " << req.body << std::endl;


	std::cout << req.method << " " << req.path << " " << req.protocol << std::endl;

	auto headers = req.headers;
	for ( auto it = headers.begin(); it != headers.end(); ++it )
		std::cout << it->first << ":" << it->second << std::endl;

	cb(req, resp);

	resp.write(socket);
}

} // namespace utils
} // namespace http
