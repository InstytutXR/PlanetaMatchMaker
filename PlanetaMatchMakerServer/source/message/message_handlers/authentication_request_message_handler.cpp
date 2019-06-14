﻿#include <boost/asio.hpp>

#include "server/server_data.hpp"
#include "server/server_constants.hpp"
#include "server/server_error.hpp"
#include "utilities/string_utility.hpp"
#include "utilities/log.hpp"
#include "../message_handle_utilities.hpp"
#include "authentication_request_message_handler.hpp"

using namespace boost;

namespace pgl {
	void authentication_request_message_handler::handle_message(const authentication_request_message& message,
		std::shared_ptr<message_handle_parameter> param) {
		try {
			reply_message_header header{
				message_type::authentication_reply,
				message_error_code::ok,
			};

			const authentication_reply_message reply{
				server_version
			};

			if (message.version == server_version) {
				const client_data client_data{
					client_address::make_from_endpoint(param->socket.remote_endpoint()),
					datetime::now()
				};

				log_with_endpoint(log_level::info, param->socket.remote_endpoint(), "Authentication succeeded.");
				const auto client_address = client_address::make_from_endpoint(param->socket.remote_endpoint());
				if (param->server_data->client_data_container().is_data_exist(client_address)) {
					param->server_data->client_data_container().update_data(client_address, client_data);
					log_with_endpoint(log_level::info, param->socket.remote_endpoint(), "Client data updated.");
				} else {
					param->server_data->client_data_container().add_data(client_address, client_data);
					log_with_endpoint(log_level::info, param->socket.remote_endpoint(), "Client data registered.");
				}
			} else {
				log_with_endpoint(log_level::error, param->socket.remote_endpoint(), "Authentication failed.");
				header.error_code = message_error_code::version_mismatch;
			}

			send(param, header, reply);
			log_with_endpoint(log_level::info, param->socket.remote_endpoint(), "Reply authentication message.");
		} catch (const system::system_error& e) {
			throw server_error(server_error_code::message_send_error, e.code().message());
		}

		if (message.version != server_version) {
			const auto extra_message = generate_string("server version: ", server_version, ", client version: ",
				message.version);
			throw server_error(server_error_code::version_mismatch, extra_message);
		}
	}
}
