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

	template<typename T>
	response& operator<<(const T& obj) { body_ << obj; return *this; }

	void status(int status) { status_ = status; }
	int  status() const { return status_; }

	void discard() { body_.str(""); }

	void header(const std::string& key, std::string& val) { headers_[key] = val; }
	const std::string& header(const std::string& key) { return headers_[key]; }

	void write(int socket);

};

}

#endif
