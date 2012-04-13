#include "commbox.hpp"

namespace JM
{
namespace Diag
{
Commbox::Commbox()
    : _toCommbox()
    , _fromCommbox()
    , _mutex()
    , _cond()
    , _physicalStreams()
    , _visualStream(new VisualStream(_toCommbox, _fromCommbox, _mutex, _cond))
    , _type(Unknow)
{

}

Commbox::~Commbox()
{

}

Commbox& Commbox::inst()
{
    static Commbox instance;
    return instance;
}

void Commbox::setType(Type type)
{
    _type = type;
}

Commbox::Type Commbox::type()
{
    return _type;
}

boost::shared_ptr<VisualStream> Commbox::visualStream()
{
    return _visualStream;
}
}
}