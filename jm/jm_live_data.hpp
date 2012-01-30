#ifndef JM_LIVE_DATA_HPP
#define JM_LIVE_DATA_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <jm/jm_types.hpp>

namespace jm {
	class live_data {
	public:
		explicit live_data(const std::string &short_name,
			const std::string &content,
			const std::string &unit,
			const std::string &default_value,
			jm::int32 command_id,
			bool enabled);
		~live_data();
		live_data(const live_data &other);
		live_data& operator=(const live_data &other);
		std::string short_name() const;
		std::string content() const;
		std::string unit() const;
		std::string default_value() const;
		jm::int32 command_id();
		bool is_enabled();
		void set_enabled(bool enabled);
	private:
		std::string _short_name;
		std::string _content;
		std::string _unit;
		std::string _default_value;
		jm::int32 _command_id;
		bool _enabled;
	};
	
	typedef boost::shared_ptr<live_data> live_data_ptr;
}

#endif