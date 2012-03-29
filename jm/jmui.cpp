#include "jmui.hpp"
#include <cstring>
#include <sstream>

namespace JM
{
	UserInterface::UserInterface()
		: _msgQueue()
		, _mutex()
		, _btnClicked()
		, _btnMutex()
		, _btnCond()
		, _menuSel()
		, _menuMutex()
	{
	}


	UserInterface::~UserInterface()
	{
		for (std::deque<JMUIMessage*>::iterator it = _msgQueue.begin(); it != _msgQueue.end(); it++)
		{
			msgFree(*it);
		}
	}

	UserInterface& UserInterface::inst()
	{
		static UserInterface instance;
		return instance;
	}

	void UserInterface::msgFree(JMUIMessage *obj)
	{
		if (obj == NULL)
			return;
		if (obj->message != NULL)
			delete []obj->message;
		delete obj;
	}

	JMUIMessage* UserInterface::popMsg()
	{
		boost::mutex::scoped_lock lock(_mutex);
		if (_msgQueue.empty())
			return NULL;

		JMUIMessage *msg = _msgQueue.front();
		_msgQueue.pop_front();

		return msg;
	}

	std::size_t UserInterface::msgCount()
	{
		boost::mutex::scoped_lock lock(_mutex);
		return _msgQueue.size();
	}

	void UserInterface::mainShow()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage *msg = new JMUIMessage;
		msg->message = NULL;
		msg->type = JM_UI_MAIN_SHOW;
		_msgQueue.push_back(msg);
		
	}

	void UserInterface::msgBoxBtnClr()
	{	
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MSG_BOX_BTN_CLR;
		msg->message = NULL;
		_msgQueue.push_back(msg);
		
	}

	void UserInterface::msgBoxAddBtn(const std::string &text)
	{		
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MSG_BOX_ADD_BTN;
		msg->message = new char[text.length() + 1];
		strcpy(msg->message, text.c_str());
		_msgQueue.push_back(msg);
		
	}

	void UserInterface::msgBoxSetMsg(const std::string &text)
	{		
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MSG_BOX_SET_MSG;
		msg->message = new char[text.length() + 1];
		strcpy(msg->message, text.c_str());
		_msgQueue.push_back(msg);
		
	}

	void UserInterface::msgBoxSetTitle(const std::string &text)
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MSG_BOX_SET_TITLE;
		msg->message = new char[text.length() + 1];
		strcpy(msg->message, text.c_str());
		_msgQueue.push_back(msg);
	}

	void UserInterface::msgBoxShow()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MSG_BOX_SHOW;
		msg->message = NULL;
		_msgQueue.push_back(msg);	
	}

	void UserInterface::msgBoxHide()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MSG_BOX_HIDE;
		msg->message = NULL;
		_msgQueue.push_back(msg);	
	}

	void UserInterface::listBoxBtnClr()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LIST_BOX_BTN_CLR;
		msg->message = NULL;
		_msgQueue.push_back(msg);	
	}

	void UserInterface::listBoxAddBtn(const std::string &text)
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LIST_BOX_ADD_BTN;
		msg->message = new char[text.length() + 1];
		strcpy(msg->message, text.c_str());
		_msgQueue.push_back(msg);
	}

	void UserInterface::listBoxAddItem(const std::string &caption, const std::string &item)
	{
		std::stringstream ss;
		ss << caption << "|" << item;

		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LIST_BOX_ADD_ITEM;
		msg->message = new char[ss.str().length() + 1];
		_msgQueue.push_back(msg);
	}

	void UserInterface::listBoxItemClr()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LIST_BOX_ITEM_CLR;
		msg->message = NULL;
		_msgQueue.push_back(msg);	
	}

	void UserInterface::listBoxShow()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LIST_BOX_SHOW;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::listBoxHide()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LIST_BOX_HIDE;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::menuItemClr()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MENU_ITEM_CLR;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::menuAddItem(const std::string &text)
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MENU_ADD_ITEM;
		msg->message = new char[text.length() + 1];
		strcpy(msg->message, text.c_str());
		_msgQueue.push_back(msg);
	}

	void UserInterface::menuShow()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_MENU_SHOW;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::tcItemClr()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_TC_ITEM_CLR;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::tcAddItem(const std::string &code, const std::string &text)
	{
		std::stringstream ss;
		ss << code << "|" << text;
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_TC_ADD_ITEM;
		msg->message = new char[ss.str().length() + 1];
		strcpy(msg->message, ss.str().c_str());
		_msgQueue.push_back(msg);
	}

	void UserInterface::tcAddBtn(const std::string &text)
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_TC_ADD_BTN;
		msg->message = new char[text.length() + 1];
		strcpy(msg->message, text.c_str());
		_msgQueue.push_back(msg);
	}

	void UserInterface::tcBtnClr()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_TC_BTN_CLR;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::tcShow()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_TC_SHOW;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::ldPrepareShow()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LD_PREPARE_SHOW;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::ldShow()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LD_SHOW;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::ldBtnClr()
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LD_BTN_CLR;
		msg->message = NULL;
		_msgQueue.push_back(msg);
	}

	void UserInterface::ldAddBtn(const std::string &text)
	{
		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LD_ADD_BTN;
		msg->message = new char[text.length() + 1];
		strcpy(msg->message, text.c_str());
		_msgQueue.push_back(msg);
	}

	void UserInterface::ldSetValue(int32_t index, const std::string &value)
	{
		std::stringstream ss;
		ss << index << "|" << value;

		boost::mutex::scoped_lock lock(_mutex);
		JMUIMessage* msg = new JMUIMessage;
		msg->type = JM_UI_LD_SET_VALUE;
		msg->message = new char[ss.str().length() + 1];
		strcpy(msg->message, ss.str().c_str());
		_msgQueue.push_back(msg);
	}

	std::string UserInterface::btnClicked(bool isBlocked)
	{
		boost::unique_lock<boost::mutex> lock(_btnMutex);

		std::string ret;

		if (isBlocked)
		{
			while (_btnClicked.empty())
			{
				_btnCond.wait(lock);
			}
			ret = _btnClicked;
			_btnClicked.clear();
		}
		else
		{
			_btnCond.timed_wait(lock, boost::posix_time::milliseconds(4));
			if (_btnClicked.empty())
			{

			}
			else
			{
				ret = _btnClicked;
				_btnClicked.clear();
			}
		}

		return ret;
	}

	void UserInterface::setBtnClicked(const std::string &text)
	{
		boost::lock_guard<boost::mutex> lock(_btnMutex);
		_btnClicked = text;
		_btnCond.notify_one();
	}

	std::string UserInterface::menuSelected()
	{
		boost::mutex::scoped_lock lock(_menuMutex);
		std::string ret = _menuSel;
		_menuSel.clear();
		return ret;
	}

	void UserInterface::setMenuSelected(const std::string &text)
	{
		boost::mutex::scoped_lock lock(_menuMutex);
		_menuSel = text;
	}

}