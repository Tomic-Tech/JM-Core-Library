#ifndef JM_UI_HPP
#define JM_UI_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <deque>
#include <boost/thread.hpp>
#include <jm/jm_types.hpp>

namespace jm {
	class ui {
	public:
		enum message_type {
			UNKNOW,
			MAIN_SHOW,
			MSG_BOX_BTN_CLR,
			MSG_BOX_ADD_BTN,
			MSG_BOX_SET_MSG,
			MSG_BOX_SET_TITLE,
			MSG_BOX_SHOW,
			MSG_BOX_HIDE,
			LIST_BOX_BTN_CLR,
			LIST_BOX_ADD_BTN,
			LIST_BOX_ITEM_CLR,
			LIST_BOX_ADD_ITEM,
			LIST_BOX_SHOW,
			LIST_BOX_HIDE,
			MENU_ITEM_CLR,
			MENU_ADD_ITEM,
			MENU_SHOW,
			TC_ITEM_CLR,
			TC_ADD_ITEM,
			TC_ADD_BTN,
			TC_BTN_CLR,
			TC_SHOW,
			LD_PREPARE_SHOW,
			LD_SHOW,
			LD_BTN_CLR,
			LD_ADD_BTN,
			LD_SET_VALUE
		};
	public:
		ui& instance();
		~ui();
		void main_show();
		void msg_box_btn_clr();
		void msg_box_add_btn(const std::string &text);
		void msg_box_set_msg(const std::string &text);
		void msg_box_set_title(const std::string &text);
		void msg_box_show();
		void msg_box_hide();
		void list_box_btn_clear();
		void list_box_add_btn(const std::string &text);
		void list_box_add_item(const std::string &caption, const std::string &item);
		void list_box_item_clr();
		void list_box_show();
		void list_box_hide();
		void menu_item_clr();
		void menu_add_item(const std::string &text);
		void menu_show();
		void tc_add_item(const std::string &code, const std::string &text);
		void tc_add_btn(const std::string &text);
		void tc_btn_clr();
		void tc_show();
		void ld_prepare_show();
		void ld_show();
		void ld_btn_clr();
		void ld_add_btn(const std::string &text);
		void ld_set_value(int32 index, const std::string &value);
		message_type pop_msg_type();
		std::string pop_msg();
	private:
		ui();
	private:
		boost::recursive_mutex _mutex;
		std::deque<message_type> _type_queue;
		std::deque<std::string> _msg_queue;
	};

	ui& ui_inst();
}

#endif