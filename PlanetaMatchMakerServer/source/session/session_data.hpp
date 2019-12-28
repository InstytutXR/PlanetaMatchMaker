#pragma once

#include "session/session_key.hpp"
#include "room/room_constants.hpp"
#include "network/endpoint_address.hpp"

namespace pgl {
	// This class need not be thread safe because one session is processed in one thread.
	// Each method may throw std::runtime_exception if errors occur.
	class session_data final {
	public:
		session_key_type generate_session_key();
		void set_hosting_room_id(room_group_index_type room_group_index, room_id_type room_id);
		void delete_hosting_room_id(room_group_index_type room_group_index, room_id_type room_id);
		void set_client_address(const endpoint_address& client_address);
		[[nodiscard]] room_group_index_type hosting_room_group_index() const;
		[[nodiscard]] room_id_type hosting_room_id() const;
		[[nodiscard]] bool is_session_key_generated() const;
		[[nodiscard]] bool is_hosting_room() const;
		[[nodiscard]] bool check_session_key(session_key_type session_key) const;
		[[nodiscard]] const endpoint_address& client_address() const;
	private:
		bool is_session_key_generated_ = false;
		session_key_type session_key_{};
		bool is_hosting_room_ = false;
		room_group_index_type hosting_room_group_index_{};
		room_id_type hosting_room_id_{};
		endpoint_address client_address_{};
	};
}
