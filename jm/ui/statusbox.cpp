#include "statusbox.hpp"
#include "message.hpp"

namespace JM
{
namespace UI
{
StatusBox::StatusBox(const std::string &title, const std::string &msg)
    : _title(title)
    , _msg(msg)
{

}

StatusBox::~StatusBox()
{
    hide();
}

void StatusBox::show()
{
    Message::inst().msgBoxBtnClr();
    Message::inst().msgBoxSetTitle(_title);
    Message::inst().msgBoxSetMsg(_msg);
    Message::inst().msgBoxShow();
}

void StatusBox::hide()
{
    Message::inst().msgBoxHide();
}
}
}