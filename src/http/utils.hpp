#ifndef __HTTP_UTILS_HPP
#define __HTTP_UTILS_HPP
#include "server.hpp"

namespace http {
namespace utils {

void process_request(int socket, server::callback_type cb);

} // namespace utils
} // namespace http

#endif
