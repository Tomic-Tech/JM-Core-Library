#include "troublecode.hpp"
#include "message.hpp"

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

void TroubleCode::addItems(const TCVector &vec)
{
    Message::inst().tcItemClr();
    for (TCVector::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        Message::inst().tcAddItem(it->first, it->second);
    }
}

void TroubleCode::addBtns(const StringVector &btns)
{
    _btns = btns;
}

std::string TroubleCode::btnClicked()
{
    return Message::inst().btnClicked(true);
}

void TroubleCode::operator()()
{
    Message::inst().tcBtnClr();
    for (StringVector::const_iterator it = _btns.begin(); it != _btns.end(); ++it)
    {
        Message::inst().tcAddBtn(*it);
    }
    Message::inst().msgBoxHide();
    Message::inst().tcItemClr();
    Message::inst().tcShow();
    firstFuncEvent();
    while (btnClickedEvent(btnClicked()));
}


}
}
