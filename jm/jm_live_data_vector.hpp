#ifndef JM_LIVE_DATA_VECTOR_HPP
#define JM_LIVE_DATA_VECTOR_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <jm/jm_types.hpp>
#include <jm/jm_live_data.hpp>

namespace jm {
	class live_data_vector : public std::vector<live_data_ptr> {
	public:
		live_data_vector();
		~live_data_vector();
		live_data_vector(const live_data_vector &other);
		live_data_vector& operator=(const live_data_vector &other);
		int32 next_enabled_index();
		void generate_enabled_index();
	private:
		int32 _current_enabled_index;
		std::vector<int32> _enabled_index_vector;
		boost::mutex _mutex;
	};

	typedef boost::shared_ptr<live_data_vector> live_data_vector_ptr;
}

#endif