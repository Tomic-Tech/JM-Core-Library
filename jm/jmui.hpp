#ifndef __JM_UI_HPP__
#define __JM_UI_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmui.h>
#include <deque>
#include <boost/thread.hpp>

namespace JM
{
    class UserInterface
    {
    public:
        static UserInterface& inst();
        void msgFree(JMUIMessage *obj);
        JMUIMessage* popMsg();
        size_t msgCount();
        void mainShow();
        void msgBoxBtnClr();
        void msgBoxAddBtn(const std::string &text);
        void msgBoxSetMsg(const std::string &text);
        void msgBoxSetTitle(const std::string &text);
        void msgBoxShow();
        void msgBoxHide();
        void listBoxBtnClr();
        void listBoxAddBtn(const std::string &text);
        void listBoxAddItem(const std::string &caption, const std::string &item);
        void listBoxItemClr();
        void listBoxShow();
        void listBoxHide();
        void menuItemClr();
        void menuAddItem(const std::string &text);
        void menuShow();
        void tcItemClr();
        void tcAddItem(const std::string &code, const std::string &text);
        void tcAddBtn(const std::string &text);
        void tcBtnClr();
        void tcShow();
        void ldPrepareShow();
        void ldShow();
        void ldBtnClr();
        void ldAddBtn(const std::string &text);
        void ldSetValue(boost::int32_t index, const std::string &value);
        std::string btnClicked(bool isBlocked);
        void setBtnClicked(const std::string &text);
        std::string menuSelected();
        void setMenuSelected(const std::string &text);
    private:
        UserInterface();
        ~UserInterface();
    private:
		std::deque<JMUIMessage*> _msgQueue;
		boost::mutex _mutex;
		std::string _btnClicked;
		boost::mutex _btnMutex;
		boost::condition_variable _btnCond;
		std::string _menuSel;
		boost::mutex _menuMutex;
    };
}

#endif