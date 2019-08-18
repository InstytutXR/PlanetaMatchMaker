#pragma once

#include <unordered_map>
#include <atomic>
#include <shared_mutex>
#include <functional>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include <boost/noncopyable.hpp>
#include <boost/call_traits.hpp>

#include "random_id_generator.hpp"

namespace pgl {
	template <typename S, typename T>
	auto member_variable_pointer_t_impl(T S::* p) -> T;

	template <typename T>
	using member_variable_pointer_variable_t = decltype(member_variable_pointer_t_impl(std::declval<T>()));

	template <typename S, auto S::* UniqueVariable>
	class unique_variables_container_impl {
		using member_t = member_variable_pointer_variable_t<decltype(UniqueVariable)>;
		using s_param_t = typename boost::call_traits<S>::param_type;
	protected:
		void add_or_update_key(s_param_t data) {
			used_keys_.insert(data.*UniqueVariable);
		}

		void remove_key(s_param_t data) {
			used_keys_.erase(data.*UniqueVariable);
		}

		bool is_unique(s_param_t data) const {
			return used_keys_.find(data.*UniqueVariable) == used_keys_.end();
		}

	private:
		std::unordered_set<member_t> used_keys_;
	};

	// A container which holds unique member variables.
	template <typename S, auto S::* ... UniqueVariables>
	class unique_variables_container
		final : boost::noncopyable, unique_variables_container_impl<S, UniqueVariables>... {
		using s_param_t = typename boost::call_traits<S>::param_type;
	public:
		void add_or_update_keys(s_param_t data) {
			(unique_variables_container_impl<S, UniqueVariables>::add_or_update_key(data), ...);
		}

		void remove_keys(s_param_t data) {
			(unique_variables_container_impl<S, UniqueVariables>::remove_key(data), ...);
		}

		bool is_unique(s_param_t data) const {
			return (unique_variables_container_impl<S, UniqueVariables>::is_unique(data) && ... && true);
		}
	};

	class unique_variable_duplication_error final : public std::runtime_error {
	public:
		unique_variable_duplication_error() : runtime_error("A unique member variable is duplicated.") {};
		using runtime_error::runtime_error;
	};

	// A thread safe container of data
	template <typename Id, typename Data, auto Data::* ... UniqueVariables>
	class thread_safe_data_container final : boost::noncopyable {
	public:
		using id_param_type = typename boost::call_traits<Id>::param_type;
		using data_param_type = typename boost::call_traits<Data>::param_type;

		[[nodiscard]] bool is_data_exist(id_param_type id) const {
			std::shared_lock lock(mutex_);
			return data_map_.find(id) != data_map_.end();
		}

		[[nodiscard]] Data get_data(id_param_type id) const {
			std::shared_lock lock(mutex_);
			return data_map_.at(id).load();
		}

		size_t size() const {
			return data_map_.size();
		}

		// unique_variable_duplication_error will be thrown if unique member variable is duplicated.
		void add_data(id_param_type id, data_param_type data) {
			std::lock_guard lock(mutex_);

			if (!unique_variables_.is_unique(data)) {
				throw unique_variable_duplication_error();
			}
			
			data_map_.emplace(id, data);
			unique_variables_.add_or_update_keys(data);
		}

		// unique_variable_duplication_error will be thrown if unique member variable is duplicated.
		Id assign_id_and_add_data(Data& data,
			std::function<void(Data&, id_param_type)>&& id_setter = [](Data&, id_param_type) {},
			std::function<Id()>&& random_id_generator = generate_random_id<Id>) {
			std::lock_guard lock(mutex_);

			if (!unique_variables_.is_unique(data)) {
				throw unique_variable_duplication_error();
			}
			
			Id id;
			do {
				id = random_id_generator();
			} while (data_map_.find(id) != data_map_.end());
			id_setter(data, id);
			data_map_.emplace(id, data);
			unique_variables_.add_or_update_keys(data);
			return id;
		}

		std::vector<Data> get_range_data(const size_t start_idx, size_t count,
			std::function<bool(data_param_type, data_param_type)>&& compare_function,
			std::function<bool(data_param_type)>&& filter_function) const {
			std::shared_lock lock(mutex_);
			std::vector<Data> data;
			data.reserve(data_map_.size());
			for (auto&& pair : data_map_) {
				if (filter_function(pair.second)) {
					data.push_back(pair.second.load());
				}
			}
			std::sort(data.begin(), data.end(), compare_function);
			count = std::min(count, data.size() >= start_idx ? data.size() - start_idx : 0);
			std::vector<Data> result(count);
			const auto end_idx_plus_one = start_idx + count;
			for (auto i = start_idx; i < end_idx_plus_one; ++i) {
				result[i - start_idx] = data[i];
			}
			return result;
		}

		// unique_variable_duplication_error will be thrown if unique member variable is duplicated.
		void update_data(id_param_type id, data_param_type data) {
			std::shared_lock lock(mutex_);

			if (!unique_variables_.is_unique(data)) {
				throw unique_variable_duplication_error();
			}

			unique_variables_.add_or_update_keys(data);
			data_map_.at(id) = data;
		}

		void remove_data(id_param_type id) {
			std::lock_guard lock(mutex_);
			auto it = data_map_.find(id);
			unique_variables_.remove_keys(it->second);
			data_map_.erase(it);
		}

	private:
		std::unordered_map<Id, std::atomic<Data>> data_map_;
		unique_variables_container<Data, UniqueVariables...> unique_variables_;
		mutable std::shared_mutex mutex_;
	};
}
