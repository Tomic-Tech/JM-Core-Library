#include "livedataprepare.h"
#include "message.h"
#include "../system/app.h"


namespace JM
{
namespace UI
{

LiveDataPrepare::LiveDataPrepare()
    : _ptr()
{
    
}

LiveDataPrepare::~LiveDataPrepare()
{
    
}

bool LiveDataPrepare::enter()
{
    std::string input = Message::inst().btn_clicked(true);
    return input.compare(".") == 0;
}

void LiveDataPrepare::set_ptr(const Diag::LiveDataVectorPtr &ptr)
{
    _ptr = ptr;
}

void LiveDataPrepare::operator()()
{
    _ptr->deploy_enabled_index();
    System::App::inst().ldVectorPtr = _ptr;
    Message::inst().ld_prepare_show();
    if (enter())
    {
        enterEvent();
    }
}

}
}
