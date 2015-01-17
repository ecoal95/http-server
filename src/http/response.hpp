#ifndef __HTTP_RESPONSE_HPP
#define __HTTP_RESPONSE_HPP

#include <sstream>
#include <map>
namespace http {

class response {
	std::stringstream body_;
	int status_;
	std::map<std::string, std::string> headers_;

	static std::map<int, const char *> statuses;
public:
	response() : status_(200) {};

	/**
	 * Add content to the response
	 *  allow chaining of operator<<
	 */
	template<typename T>
	response& operator<<(const T& obj) { body_ << obj; return *this; }

	/** Get status */
	int  status() const { return status_; }

	/** Set status */
	void status(int status) { status_ = status; }

	/** Discard current response */
	void discard() { body_.str(""); }

	/** Add or set a response header */
	void header(const std::string& key, const std::string& val)       { headers_[key] = val; }

	/** Get a response header */
	const std::string& header(const std::string& key)           { return headers_[key]; }

	/** Write response to a socket */
	void write(int socket, bool close_at_end = true) { write_to(socket, close_at_end); } // alias
	void write_to(int socket, bool close_at_end = true);

};

}

#endif
