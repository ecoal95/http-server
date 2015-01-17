#include "utils.hpp"
#include "request.hpp"
#include "response.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <socketutils/socketutils.hpp>

#define REQUEST_HEADER_LINE_MAX_SIZE 1024
#define RESPONSE_STATIC_CHUNK_SIZE 1024

namespace http {
namespace utils {

bool file_exists(const char *name) {
	FILE *file = fopen(name, "r");

	if ( file == NULL )
		return false;

	fclose(file);
	return true;
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

void write_file(const char *file_name, int socket) {
	char buffer[RESPONSE_STATIC_CHUNK_SIZE];
	size_t read;
	FILE *file = fopen(file_name, "r");

	if ( file == NULL )
		throw runtime_error("File could not be read.");

	while ( (read = fread(buffer, 1, RESPONSE_STATIC_CHUNK_SIZE, file)) ) {
		std::clog << "Read " << read << " bytes: " << buffer << std::endl;
		socketutils::safe_send(socket, buffer, read, 0);
	}
}




} // namespace utils
} // namespace http
