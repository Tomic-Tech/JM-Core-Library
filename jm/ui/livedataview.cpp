#include "livedataview.h"
#include "../System/app.h"
#include "message.h"

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

void LiveDataView::set_ptr(const Diag::LiveDataVectorPtr &ptr)
{
    _ptr = ptr;
}

void LiveDataView::add_btns(const StringVector &btns)
{
    _btns = btns;
}

void LiveDataView::operator() ()
{
    System::App::inst().ldVectorPtr = _ptr;
    _ptr->deploy_enabled_index();
    _ptr->deploy_showed_index();
    Message::inst().ld_show();
    
    Message::inst().ld_btn_clr();
    for (StringVector::iterator it = _btns.begin(); it != _btns.end(); ++it)
    {
        Message::inst().ld_add_btn(*it);
    }
    
    while (btnClickedEvent(Message::inst().btn_clicked(false)));
}

}
}
