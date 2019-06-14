﻿#include "update_room_status_request_message_handler.hpp"
#include "utilities/string_utility.hpp"
#include "../message_handle_utilities.hpp"

namespace pgl {

	void update_room_status_request_message_handler::handle_message(const update_room_status_request_message& message,
		std::shared_ptr<message_handle_parameter> param) {
		update_room_status_reply_message reply{};

		check_remote_endpoint_existence<message_type::update_room_status_reply>(param, reply);

		check_room_group_existence<message_type::update_room_status_reply>(param, message.group_index, reply);
		auto& room_data_container = param->server_data->get_room_data_container(message.group_index);

		check_room_existence<message_type::update_room_status_reply
		>(param, room_data_container, message.room_id, reply);
		auto room_data = room_data_container.get_data(message.room_id);

		if (room_data.host_address != client_address::make_from_endpoint(param->socket.remote_endpoint())) {
			const reply_message_header header{
				message_type::update_room_status_reply,
				message_error_code::permission_denied
			};
			send(param, header, reply);
			const auto extra_message = generate_string("The client is not host of requested room.");
			throw server_error(server_error_code::room_permission_error, extra_message);
		}

		switch (message.status) {
			case update_room_status_request_message::status::open:
				room_data.flags |= room_flags_bit_mask::is_open;
				room_data_container.update_data(room_data);
				break;
			case update_room_status_request_message::status::close:
				room_data.flags &= ~room_flags_bit_mask::is_open;
				room_data_container.update_data(room_data);
				break;
			case update_room_status_request_message::status::remove:
				room_data_container.remove_data(room_data.room_id);
				break;
			default:
				break;
		}

		const reply_message_header header{
			message_type::update_room_status_reply,
			message_error_code::ok
		};
		send(param, header, reply);
	}
}
