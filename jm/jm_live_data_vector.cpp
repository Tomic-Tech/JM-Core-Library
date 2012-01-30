#include "jm_live_data_vector.hpp"

namespace jm {
	live_data_vector::live_data_vector()
		: _current_enabled_index(0)
		, _mutex() {
	}

	live_data_vector::~live_data_vector() {
	}

	live_data_vector::live_data_vector(const live_data_vector &other) {
		clear();
		for (const_iterator it = other.begin(); it != other.end(); it++) {
			push_back(*it);
		}
	}

	live_data_vector& live_data_vector::operator=(const live_data_vector &other) {
		clear();
		for (const_iterator it = other.begin(); it != other.end(); it++)
			push_back(*it);
		return *this;
	}

	int32 live_data_vector::next_enabled_index() {
		boost::mutex::scoped_lock scopedLock(_mutex);
		if (_enabled_index_vector.empty())
			return -1;

		int32 ret = _enabled_index_vector[_current_enabled_index];
		_current_enabled_index++;
		if (_current_enabled_index > static_cast<int32>(_enabled_index_vector.size() - 1))
			_current_enabled_index = 0;
		return ret;
	}

	void live_data_vector::generate_enabled_index() {
		boost::mutex::scoped_lock scopedLock(_mutex);
		_enabled_index_vector.clear();
		for (size_t i = 0; i < size(); ++i) {
			if (at(i)->is_enabled())
				_enabled_index_vector.push_back(i);
		}
	}
}