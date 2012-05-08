#include "statusbox.h"
#include "message.h"

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
    Message::inst().msg_box_btn_clr();
    Message::inst().msg_box_set_title(_title);
    Message::inst().msg_box_set_msg(_msg);
    Message::inst().msg_box_show();
}

void StatusBox::hide()
{
    Message::inst().msg_box_hide();
}

}
}
