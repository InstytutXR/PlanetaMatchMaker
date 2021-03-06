#include "server_data.hpp"

namespace pgl {

	server_data::server_data(std::vector<room_group_name_t>&& room_groups):
		room_group_data_list_(room_groups.size()), room_data_container_list_(room_groups.size()) {
		room_group_data_list_.resize(room_groups.size());
		std::transform(room_groups.begin(), room_groups.end(), room_group_data_list_.begin(),
			[](const auto& room_group_name) { return room_group_data{room_group_name}; });
	}

	size_t server_data::room_group_count() const { return room_data_container_list_.size(); }

	bool server_data::is_valid_room_group_index(const room_group_index_t room_group_index) const {
		return 0 <= room_group_index && room_group_index < room_group_count();
	}

	const server_data::room_group_data_list_type& server_data::get_room_data_group_list() const {
		return room_group_data_list_;
	}

	const server_data::room_data_container_type& server_data::get_room_data_container(
		const room_group_index_t room_group_idx) const { return room_data_container_list_.at(room_group_idx); }

	const player_name_container& server_data::get_player_name_container() const { return player_name_container_; }

	server_data::room_data_container_type& server_data::
	get_room_data_container(const room_group_index_t room_group_idx) {
		return room_data_container_list_.at(room_group_idx);
	}

	player_name_container& server_data::get_player_name_container() { return player_name_container_; }
}
