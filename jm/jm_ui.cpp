#include "jm_ui.hpp"
#include <sstream>

namespace jm {
	ui& ui::instance() {
		static ui inst;
		return inst;
	}

	ui::ui() {
	}

	ui::~ui() {
	}

	ui::message_type ui::pop_msg_type() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		if (_type_queue.empty())
			return ui::UNKNOW;
		ui::message_type type = _type_queue.front();
		_type_queue.pop_front();
		return type;
	}

	std::string ui::pop_msg() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		if (_msg_queue.empty())
			return std::string();
		std::string msg = _msg_queue.front();
		_msg_queue.pop_front();
		return msg;
	}

	void ui::main_show() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MAIN_SHOW);
		_msg_queue.push_back(std::string());
	}

	void ui::msg_box_btn_clr() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MSG_BOX_BTN_CLR);
		_msg_queue.push_back(std::string());
	}

	void ui::msg_box_add_btn(const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MSG_BOX_ADD_BTN);
		_msg_queue.push_back(text);
	}

	void ui::msg_box_set_msg(const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MSG_BOX_SET_MSG);
		_msg_queue.push_back(text);
	}

	void ui::msg_box_set_title(const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MSG_BOX_SET_TITLE);
		_msg_queue.push_back(text);
	}

	void ui::msg_box_show() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MSG_BOX_SHOW);
		_msg_queue.push_back(std::string());
	}

	void ui::msg_box_hide() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MSG_BOX_HIDE);
		_msg_queue.push_back(std::string());
	}

	void ui::list_box_btn_clear() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LIST_BOX_BTN_CLR);
		_msg_queue.push_back(std::string());
	}

	void ui::list_box_add_btn(const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LIST_BOX_ADD_BTN);
		_msg_queue.push_back(text);
	}

	void ui::list_box_add_item(const std::string &caption, const std::string &item) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LIST_BOX_ADD_ITEM);
		std::stringstream ss;
		ss << caption << "|" << item;
		_msg_queue.push_back(ss.str());
	}

	void ui::list_box_item_clr() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LIST_BOX_ITEM_CLR);
		_msg_queue.push_back(std::string());
	}

	void ui::list_box_show() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LIST_BOX_SHOW);
		_msg_queue.push_back(std::string());
	}

	void ui::list_box_hide() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LIST_BOX_HIDE);
		_msg_queue.push_back(std::string());
	}

	void ui::menu_item_clr() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MENU_ITEM_CLR);
		_msg_queue.push_back(std::string());
	}

	void ui::menu_add_item(const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MENU_ADD_ITEM);
		_msg_queue.push_back(text);
	}

	void ui::menu_show() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::MENU_SHOW);
		_msg_queue.push_back(std::string());
	}

	void ui::tc_add_item(const std::string &code, const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::TC_ADD_ITEM);
		std::stringstream ss;
		ss << code << "|" << text;
		_msg_queue.push_back(ss.str());
	}

	void ui::tc_add_btn(const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::TC_ADD_BTN);
		_msg_queue.push_back(text);
	}

	void ui::tc_btn_clr() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::TC_BTN_CLR);
		_msg_queue.push_back(std::string());
	}

	void ui::tc_show() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::TC_SHOW);
		_msg_queue.push_back(std::string());
	}

	void ui::ld_prepare_show() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LD_PREPARE_SHOW);
		_msg_queue.push_back(std::string());
	}


	void ui::ld_show() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LD_SHOW);
		_msg_queue.push_back(std::string());
	}

	void ui::ld_btn_clr() {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LD_BTN_CLR);
		_msg_queue.push_back(std::string());
	}

	void ui::ld_add_btn(const std::string &text) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LD_ADD_BTN);
		_msg_queue.push_back(text);
	}

	void ui::ld_set_value(int32 index, const std::string &value) {
		boost::recursive_mutex::scoped_lock lock(_mutex);
		_type_queue.push_back(ui::LD_SET_VALUE);
		std::stringstream ss;
		ss << index << "|" << value;
		_msg_queue.push_back(ss.str());
	}
}