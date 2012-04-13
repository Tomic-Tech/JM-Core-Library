#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <string>
#include <deque>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <jm/types.hpp>

namespace JM
{
namespace UI
{
class Message
{
public:
    enum Type
    {
        MainShow,
        MsgBoxBtnClr,
        MsgBoxAddBtn,
        MsgBoxSetMsg,
        MsgBoxSetTitile,
        MsgBoxShow,
        MsgBoxHide,
        ListBoxBtnClr,
        ListBoxAddBtn,
        ListBoxItemClr,
        ListBoxAddItem,
        ListBoxShow,
        ListBoxHide,
        MenuItemClr,
        MenuAddItem,
        MenuShow,
        TcItemClr,
        TcAddItem,
        TcAddBtn,
        TcBtnClr,
        TcShow,
        LdPrepareShow,
        LdShow,
        LdBtnClr,
        LdAddBtn,
        LdSetValue,
        Unknow
    };

    struct Msg
    {
        Type type;
        std::string msg;
    };

public:
    static JMCORE_API Message& inst();
    JMCORE_API Msg* popMsg();
    JMCORE_API std::size_t msgCount();
    JMCORE_API void mainShow();
    JMCORE_API void msgBoxBtnClr();
    JMCORE_API void msgBoxAddBtn(const std::string &text);
    JMCORE_API void msgBoxSetMsg(const std::string &text);
    JMCORE_API void msgBoxSetTitle(const std::string &text);
    JMCORE_API void msgBoxShow();
    JMCORE_API void msgBoxHide();
    JMCORE_API void listBoxBtnClr();
    JMCORE_API void listBoxAddBtn(const std::string &text);
    JMCORE_API void listBoxAddItem(const std::string &caption, const std::string &item);
    JMCORE_API void listBoxItemClr();
    JMCORE_API void listBoxShow();
    JMCORE_API void listBoxHide();
    JMCORE_API void menuItemClr();
    JMCORE_API void menuAddItem(const std::string &text);
    JMCORE_API void menuAddItems(const std::vector<std::string> &menus);
    JMCORE_API void menuShow();
    JMCORE_API void tcItemClr();
    JMCORE_API void tcAddItem(const std::string &code, const std::string &text);
    JMCORE_API void tcAddBtn(const std::string &text);
    JMCORE_API void tcBtnClr();
    JMCORE_API void tcShow();
    JMCORE_API void ldPrepareShow();
    JMCORE_API void ldShow();
    JMCORE_API void ldBtnClr();
    JMCORE_API void ldAddBtn(const std::string &text);
    void ldSetValue(boost::int32_t index, const std::string &value);
    JMCORE_API std::string btnClicked(bool isBlocked);
    JMCORE_API void setBtnClicked(const std::string &text);
    JMCORE_API std::string menuSelected();
    JMCORE_API void setMenuSelected(const std::string &text);
private:
    Message();
    ~Message();
private:
    std::deque<Msg*> _msgQueue;
    boost::mutex _mutex;
    std::string _btnClicked;
    boost::mutex _btnMutex;
    boost::condition_variable _btnCond;
    std::string _menuSel;
    boost::mutex _menuMutex;

};
}
}

#endif