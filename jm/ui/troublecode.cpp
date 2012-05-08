#include "troublecode.h"
#include "message.h"

namespace JM
{
namespace UI
{

TroubleCode::TroubleCode()
: _btns()
{

}

TroubleCode::~TroubleCode()
{
    
}

TroubleCode::TroubleCode(const TroubleCode & tc)
    : _btns(tc._btns)
{

}

TroubleCode & TroubleCode::operator=(const TroubleCode &tc)
{
    _btns = tc._btns;
    return *this;
}

void TroubleCode::add_items(const TCVector &vec)
{
    Message::inst().tc_item_clr();
    for (TCVector::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        Message::inst().tc_add_item(it->first, it->second);
    }
}

void TroubleCode::add_btns(const StringVector &btns)
{
    _btns = btns;
}

std::string TroubleCode::btn_clicked()
{
    return Message::inst().btn_clicked(true);
}

void TroubleCode::operator() ()
{
    Message::inst().tc_btn_clr();
    for (StringVector::iterator it = _btns.begin(); it != _btns.end(); ++it)
    {
        Message::inst().tc_add_btn(*it);
    }
    Message::inst().msg_box_hide();
    Message::inst().tc_item_clr();
    Message::inst().tc_show();
    firstFuncEvent();
    while (btnClickedEvent(btn_clicked()));
}

}
}
