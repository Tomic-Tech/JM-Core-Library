#include "boxfactory.hpp"
#include "jm/diag/v1/boxversion.hpp"

namespace JM
{
namespace Diag
{

BoxFactory::BoxFactory()
    : _boxHash()
{

}

BoxFactory::~BoxFactory()
{

}

BoxFactory& BoxFactory::inst()
{
    static BoxFactory instance;
    return instance;
}

void BoxFactory::setVersion(Version ver)
{
    BoxVersion *box = NULL;
    switch(ver)
    {
    case V1:
        if (_boxHash.find(ver) != _boxHash.end())
        {
            box = _boxHash[ver].get();
        }
        if (box != NULL)
            break;
        box = new V1::BoxVersion();
        _boxHash[ver].reset(box);
        break;
    default:
        break;
    }
    _ver = ver;
}

BoxVersionPtr BoxFactory::getBox()
{
    if (_boxHash.find(_ver) != _boxHash.end())
        return _boxHash[_ver];
    return BoxVersionPtr();
}

}
}