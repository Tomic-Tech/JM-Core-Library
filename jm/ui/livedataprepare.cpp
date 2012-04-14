#include "livedataprepare.hpp"
#include "../system/app.hpp"
#include "message.hpp"

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
    std::string input = Message::inst().btnClicked(true);
    return input.compare(".") == 0;
}

void LiveDataPrepare::setPtr(const Diag::LiveDataVectorPtr &ptr)
{
    _ptr = ptr;
}

void LiveDataPrepare::operator()()
{
    _ptr->deployEnabledIndex();
    System::app().ldVecPtr = _ptr;
    Message::inst().ldPrepareShow();
    if (enter())
        enterEvent();
}

}
}