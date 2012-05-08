#include "boxstream.h"

namespace JM
{
namespace Diag
{

BoxStream::BoxStream()
: _toCommbox()
, _fromCommbox()
, _mutex()
, _cond()
, _serialPortStream(new RealStream<JM::Asio::serial_port>(_toCommbox, _fromCommbox, _mutex, _cond))
, _virtualStream(new VirtualStream(_toCommbox, _fromCommbox, _mutex, _cond))
, _type(Unknow)
{
    
}

BoxStream::~BoxStream()
{

}

BoxStream &BoxStream::inst()
{
    static BoxStream instance;
    return instance;
}

void BoxStream::set_type(RealType t)
{
    _type = t;
    _serialPortStream->stop();
    switch(t)
    {
    case SerialPort:
        _serialPortStream->start();
        break;
    default:
        break;
    }
}

JM::Asio::serial_port_ptr BoxStream::serial_port()
{
    if (_type == SerialPort)
    {
        return _serialPortStream->real();
    }
    return JM::Asio::serial_port_ptr();
}

BoxStream::RealType BoxStream::type()
{
    return _type;
}

VirtualStreamPtr BoxStream::vs()
{
    return _virtualStream;
}

}
}
