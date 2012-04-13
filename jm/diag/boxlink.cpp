#include "boxlink.hpp"

namespace JM
{
namespace Diag
{
BoxLink::BoxLink()
    : _ptr(NULL)
{

}

BoxLink::~BoxLink()
{
    if (_ptr != NULL)
        delete _ptr;
}

void BoxLink::setProtocol(void *p)
{
    if (_ptr != NULL)
        delete _ptr;
    _ptr = p;
}
}
}