#include "room_data.hpp"

namespace pgl {
	std::function<bool(const room_data&, const room_data&)> get_room_data_compare_function(
		const room_data_sort_kind sort_kind) {
		switch (sort_kind) {
		case room_data_sort_kind::name_ascending:
			return [](const room_data& left, const room_data& right) { return left.name < right.name; };
		case room_data_sort_kind::name_descending:
			return [](const room_data& left, const room_data& right) { return left.name > right.name; };
		case room_data_sort_kind::create_datetime_ascending:
			return [](const room_data& left, const room_data& right) { return left.create_datetime < right.create_datetime; };
		case room_data_sort_kind::create_datetime_descending:
			return [](const room_data& left, const room_data& right) { return left.create_datetime > right.create_datetime; };
		default:
			throw std::runtime_error("Invalid room_data_sort_kind.");
		}
	}
}