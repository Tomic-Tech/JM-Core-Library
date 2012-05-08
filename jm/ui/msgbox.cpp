#include "msgbox.h"
#include "message.h"

namespace JM
{
namespace UI
{

MsgBox::MsgBox(const std::string &title, const std::string &msg)
: _title(title)
, _msg(msg)
, _btns()
{
}

MsgBox::~MsgBox()
{
    Message::inst().msg_box_hide();
}

void MsgBox::add_btns(const StringVector &btns)
{
    _btns = btns;
}

std::string MsgBox::exec()
{
    Message::inst().msg_box_btn_clr();
    for (StringVector::iterator it = _btns.begin(); it != _btns.end(); ++it)
    {
        Message::inst().msg_box_add_btn(*it);
    }
    Message::inst().msg_box_set_title(_title);
    Message::inst().msg_box_set_msg(_msg);
    Message::inst().msg_box_show();
    return Message::inst().btn_clicked(true);
}



}
}
