﻿#include "server/server_data.hpp"

#include "async/read_write.hpp"
#include "async/timer.hpp"
#include "message/messages.hpp"
#include "server/server_error.hpp"
#include "client/client_address.hpp"
#include "utilities/log.hpp"
#include "datetime/datetime.hpp"
#include "create_room_request_message_handler.hpp"

using namespace boost;

namespace pgl {

	void create_room_request_message_handler::handle_message(const create_room_request_message& message,
	                                                         message_handle_parameter& param) {
		reply_message_header header{
			message_type::create_room_reply,
			message_error_code::ok
		};

		create_room_reply reply{};

		const auto client_address = client_address::make_from_endpoint(param.socket.remote_endpoint());
		if (!param.server_data->client_data_container().is_data_exist(client_address)) {
			header.error_code = message_error_code::permission_denied;
			try {
				execute_timed_async_operation(param.io_service, param.socket, param.timeout_seconds, [&]()
				{
					packed_async_write(param.socket, param.yield, header, reply);
				});
			} catch (const system::system_error& e) {
				throw server_error(server_error_code::message_send_error, e.code().message());
			}
			throw server_error(server_error_code::permission_error);
		}

		if (param.server_data->is_valid_room_group_index(message.group_index)) {
			const auto extra_message = generate_string("Range of valid room group index is 0 to ",
			                                           param.server_data->room_group_count(), " but \"",
			                                           message.group_index, "\" is requested.");
			header.error_code = message_error_code::room_group_index_out_of_range;
			try {
				execute_timed_async_operation(param.io_service, param.socket, param.timeout_seconds, [&]()
				{
					packed_async_write(param.socket, param.yield, header, list_room_reply{});
				});
			} catch (const system::system_error& e) {
				throw server_error(server_error_code::message_send_error, e.code().message());
			}

			throw server_error(server_error_code::room_group_index_out_of_range, extra_message);
		}

		// Create room
		room_data room_data{
			{}, // assign in room_data_container.assign_id_and_add_data(room_data)
			message.name,
			message.flags,
			message.password,
			message.max_player_count,
			datetime::now(),
			client_address::make_from_endpoint(param.socket.remote_endpoint()),
			1
		};
		reply.room_id = param.server_data->get_room_data_container(message.group_index).assign_id_and_add_data(
			room_data);
		log_with_endpoint(log_level::info, param.socket.remote_endpoint(), "New room \"", room_data.name,
		                  "\" is created in group ", message.group_index, " with id: ", reply.room_id);

		try {
			execute_timed_async_operation(param.io_service, param.socket, param.timeout_seconds, [&]()
			{
				packed_async_write(param.socket, param.yield, header, reply);
			});
		} catch (const system::system_error& e) {
			throw server_error(server_error_code::message_send_error, e.code().message());
		}
	}
}
