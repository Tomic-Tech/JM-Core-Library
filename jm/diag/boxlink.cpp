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

}

void BoxLink::setProtocol(void *p)
{
    _ptr = p;
}
}
}
