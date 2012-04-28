#include "livedataview.hpp"
#include "../system/app.hpp"

namespace JM
{
namespace UI
{
LiveDataView::LiveDataView()
    : _ptr()
    , _btns()
{
}

LiveDataView::~LiveDataView()
{

}

void LiveDataView::setPtr(const Diag::LiveDataVectorPtr &ptr)
{
    _ptr = ptr;
}

void LiveDataView::addBtns(const StringVector &btns)
{
    _btns = btns;
}

void LiveDataView::operator ()()
{
    System::app().ldVecPtr = _ptr;
    _ptr->deployEnabledIndex();
    _ptr->deployShowedIndex();
    Message::inst().ldShow();

    Message::inst().ldBtnClr();
    for (StringVector::const_iterator it = _btns.begin(); it != _btns.end(); ++it)
    {
        Message::inst().ldAddBtn(*it);
    }

    while (btnClickedEvent(Message::inst().btnClicked(false)));
}

}
}
