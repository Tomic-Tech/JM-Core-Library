#include "jm_live_data.hpp"

namespace jm {

	live_data::live_data(const std::string &short_name,
		const std::string &content,
		const std::string &unit,
		const std::string &default_value,
		int32 command_id,
		bool enabled)
		: _short_name(short_name)
		, _content(content)
		, _unit(unit)
		, _default_value(default_value)
		, _command_id(command_id)
		, _enabled(enabled) {
	}

	live_data::~live_data() {
	}

	live_data::live_data(const live_data &other)
		: _short_name(other._short_name)
		, _content(other._content)
		, _unit(other._unit)
		, _default_value(other._default_value)
		, _command_id(other._command_id)
		, _enabled(other._enabled) {
	}

	live_data& live_data::operator=(const live_data &other) {
		_short_name = other._short_name;
		_content = other._content;
		_unit = other._unit;
		_default_value = other._default_value;
		_command_id = other._command_id;
		_enabled = other._enabled;
		return *this;
	}

	std::string live_data::short_name() const {
		return _short_name;
	}

	std::string live_data::content() const {
		return _content;
	}

	std::string live_data::unit() const {
		return _unit;
	}

	std::string live_data::default_value() const {
		return _default_value;
	}

	int32 live_data::command_id() {
		return _command_id;
	}

	bool live_data::is_enabled() {
		return _enabled;
	}

	void live_data::set_enabled(bool enabled) {
		_enabled = enabled;
	}
}