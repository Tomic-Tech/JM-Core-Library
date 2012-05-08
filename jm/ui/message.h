#ifndef __JM_UI_MESSAGE_H__
#define __JM_UI_MESSAGE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace UI
{

class Message : public boost::noncopyable
{
public:

    enum MsgType
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
        MsgType type;
        std::string msg;
    };

public:
    static JMCORE_API Message& inst();
    JMCORE_API Msg* pop_msg();
    JMCORE_API std::size_t msg_count();
    JMCORE_API void main_show();
    JMCORE_API void msg_box_btn_clr();
    JMCORE_API void msg_box_add_btn(const std::string &text);
    JMCORE_API void msg_box_set_msg(const std::string &text);
    JMCORE_API void msg_box_set_title(const std::string &text);
    JMCORE_API void msg_box_show();
    JMCORE_API void msg_box_hide();
    JMCORE_API void list_box_btn_clr();
    JMCORE_API void list_box_add_btn(const std::string &text);
    JMCORE_API void list_box_add_item(const std::string &caption, const std::string &item);
    JMCORE_API void list_box_item_clr();
    JMCORE_API void list_box_show();
    JMCORE_API void list_box_hide();
    JMCORE_API void menu_item_clr();
    JMCORE_API void menu_add_item(const std::string &text);
    JMCORE_API void menu_add_items(const std::vector<std::string> &menus);
    JMCORE_API void menu_show();
    JMCORE_API void tc_item_clr();
    JMCORE_API void tc_add_item(const std::string &code, const std::string &text);
    JMCORE_API void tc_add_btn(const std::string &text);
    JMCORE_API void tc_btn_clr();
    JMCORE_API void tc_show();
    JMCORE_API void ld_prepare_show();
    JMCORE_API void ld_show();
    JMCORE_API void ld_btn_clr();
    JMCORE_API void ld_add_btn(const std::string &text);
    void ld_set_value(boost::int32_t index, const std::string &value);
    JMCORE_API std::string btn_clicked(bool isBlocked);
    JMCORE_API void set_btn_clicked(const std::string &text);
    JMCORE_API std::string menu_selected();
    JMCORE_API void set_menu_selected(const std::string &text);
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

#endif /* __JM_UI_MESSAGE_H__ */
