#include "request.hpp"
#include "utils.hpp"

#define REQUEST_HEADER_LINE_MAX_SIZE 1024

namespace http {
/** Parse the whole request */
void request::parse() {
	size_t ret;

	char buff[REQUEST_HEADER_LINE_MAX_SIZE];
	char *str = NULL, *tmp;
	char c;

	bool end = false;
	bool check_next_newline = false;
	bool last_scan_header_end = true;

	std::string request_type;
	std::vector<std::string> tmp_headers = {""};
	std::string body;

	while ( !end && (ret = recv(socket_, buff, REQUEST_HEADER_LINE_MAX_SIZE - 1, 0)) > 0 ) {
		buff[ret] = '\0';
		tmp = str = buff;

		if ( check_next_newline ) { // if last header end could got chunked
			if ( *str == '\n' ) { // and really was a header end
				str++;
				tmp++;
				tmp_headers.push_back("");
			} else { // append last character
				tmp_headers.back() += '\r';
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
					tmp_headers.back() += tmp;
					tmp_headers.push_back("");
					// tmp_headers scanned successfully a header
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

			tmp_headers.back() += tmp;
		}
	}

	// Remove empty header
	if ( tmp_headers.back().empty() )
		tmp_headers.pop_back();

	utils::parse_request_first_line(*this, tmp_headers.front());

	tmp_headers.erase(tmp_headers.begin());

	headers = utils::process_headers(tmp_headers);

	body = str - 1;

	// TODO: keep reading body depending on response type

}
}// namespace http
