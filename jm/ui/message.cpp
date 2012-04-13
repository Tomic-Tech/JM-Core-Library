#include "message.hpp"
#include <sstream>

namespace JM
{
namespace UI
{

Message::Message()
    : _msgQueue()
    , _mutex()
    , _btnClicked()
    , _btnMutex()
    , _btnCond()
    , _menuSel()
    , _menuMutex()
{

}

Message::~Message()
{
    for (std::deque<Msg*>::iterator it = _msgQueue.begin(); it != _msgQueue.end(); ++it)
    {
        delete *it;
    }
}

Message& Message::inst()
{
    static Message instance;
    return instance;
}

Message::Msg* Message::popMsg()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    if (_msgQueue.empty())
        return NULL;

    Message::Msg *msg = _msgQueue.front();
    _msgQueue.pop_front();

    return msg;
}

std::size_t Message::msgCount()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    return _msgQueue.size();
}

void Message::mainShow()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg *msg = new Message::Msg;
    msg->msg = std::string();
    msg->type = MainShow;
    _msgQueue.push_back(msg);

}

void Message::msgBoxBtnClr()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MsgBoxBtnClr;
    msg->msg = std::string();
    _msgQueue.push_back(msg);

}

void Message::msgBoxAddBtn(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MsgBoxAddBtn;
    msg->msg = text;
    _msgQueue.push_back(msg);

}

void Message::msgBoxSetMsg(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MsgBoxSetMsg;
    msg->msg = text;

    _msgQueue.push_back(msg);

}

void Message::msgBoxSetTitle(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MsgBoxSetTitile;
    msg->msg = text;

    _msgQueue.push_back(msg);
}

void Message::msgBoxShow()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MsgBoxShow;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::msgBoxHide()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MsgBoxHide;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::listBoxBtnClr()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = ListBoxBtnClr;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::listBoxAddBtn(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = ListBoxAddBtn;
    msg->msg = text;

    _msgQueue.push_back(msg);
}

void Message::listBoxAddItem(const std::string &caption, const std::string &item)
{
    std::stringstream ss;
    ss << caption << "|" << item;

    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = ListBoxAddItem;
    msg->msg = ss.str();
    _msgQueue.push_back(msg);
}

void Message::listBoxItemClr()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = ListBoxItemClr;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::listBoxShow()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = ListBoxShow;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::listBoxHide()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = ListBoxHide;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::menuItemClr()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MenuItemClr;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::menuAddItem(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MenuAddItem;
    msg->msg = text;

    _msgQueue.push_back(msg);
}

void Message::menuAddItems(const std::vector<std::string> &menus)
{
    for (std::vector<std::string>::const_iterator it = menus.cbegin();
        it != menus.cend(); ++it)
    {
        menuAddItem(*it);
    }
}

void Message::menuShow()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = MenuShow;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::tcItemClr()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = TcItemClr;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::tcAddItem(const std::string &code, const std::string &text)
{
    std::stringstream ss;
    ss << code << "|" << text;
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = TcAddItem;
    msg->msg = ss.str();

    _msgQueue.push_back(msg);
}

void Message::tcAddBtn(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = TcAddBtn;
    msg->msg = text;

    _msgQueue.push_back(msg);
}

void Message::tcBtnClr()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = TcBtnClr;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::tcShow()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = TcShow;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::ldPrepareShow()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = LdPrepareShow;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::ldShow()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = LdShow;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::ldBtnClr()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = LdBtnClr;
    msg->msg = std::string();
    _msgQueue.push_back(msg);
}

void Message::ldAddBtn(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = LdAddBtn;
    msg->msg = text;

    _msgQueue.push_back(msg);
}

void Message::ldSetValue(int32_t index, const std::string &value)
{
    std::stringstream ss;
    ss << index << "|" << value;

    boost::unique_lock<boost::mutex> lock(_mutex);
    Message::Msg* msg = new Message::Msg;
    msg->type = LdSetValue;
    msg->msg = ss.str();

    _msgQueue.push_back(msg);
}

std::string Message::btnClicked(bool isBlocked)
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

void Message::setBtnClicked(const std::string &text)
{
    boost::lock_guard<boost::mutex> lock(_btnMutex);
    _btnClicked = text;
    _btnCond.notify_one();
}

std::string Message::menuSelected()
{
    boost::unique_lock<boost::mutex> lock(_menuMutex);
    std::string ret = _menuSel;
    _menuSel.clear();
    return ret;
}

void Message::setMenuSelected(const std::string &text)
{
    boost::unique_lock<boost::mutex> lock(_menuMutex);
    _menuSel = text;
}
}
}