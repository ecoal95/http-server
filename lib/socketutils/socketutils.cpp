#include "socketutils.hpp"

namespace socketutils {

void safe_send(int socket, const char *buffer, size_t size, int flags) {
	size_t sent = 0;
	const char *original_buffer = buffer;

	do {
		// size - sent = remaining size
		sent += send(socket, buffer, size - sent, flags);
		buffer = original_buffer + sent;
	} while ( sent != size );
}

}
