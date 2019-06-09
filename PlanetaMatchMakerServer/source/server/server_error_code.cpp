#include "server_error_code.hpp"

namespace pgl {
	std::string get_server_error_message(const server_error_code error_code) {
		switch (error_code) {
		case server_error_code::ok:
			return "Ok.";
		case server_error_code::acception_failed:
			return "Failed to accept new client.";
		case server_error_code::message_reception_timeout:
			return "Failed to receive a message because of time out.";
		case server_error_code::message_header_reception_error:
			return "Failed to receive a message header.";
		case server_error_code::message_body_reception_error:
			return "Failed to receive a message body.";
		case server_error_code::invalid_message_type:
			return "The message type is invalid.";
		case server_error_code::message_type_mismatch:
			return "The message type does not match to expected one.";
		case server_error_code::version_mismatch:
			return "The client version does not match to the server version.";
		case server_error_code::message_send_error:
			return "Failed to send a message to client.";
		case server_error_code::permission_error:
			return "A client without permission try to connect.";
		default:
			return "Unknown";
		}
	}

	std::ostream& operator<<(std::ostream& os, const server_error_code& error_code) {
		os << get_server_error_message(error_code);
		return os;
	}
}