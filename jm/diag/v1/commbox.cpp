#include "commbox.h"

namespace JM
{
namespace Diag
{
namespace V1
{

Commbox::Commbox()
: _shared(new Shared())
, _w80(new w80::Box(_shared))
, _c168(new C168::Box(_shared))
, _currentBox(_c168.get())
{
    _shared->buffID = 0;
    _shared->cnn = CN_UNKNOW;
    _shared->isDB20 = false;
    _shared->isDoNow = true;
    _shared->lastError = 0;
    _shared->nextAddress = 0;
    _shared->reqByteToByte = 500000;
    _shared->reqWaitTime = 500000;
    _shared->resByteToByte = 500000;
    _shared->resWaitTime = 500000;
}

Commbox::~Commbox()
{
    
}

boost::system::error_code Commbox::open(boost::system::error_code &ec)
{
    while (true)
    {
        if (_currentBox == _c168.get())
        {
            if (!_c168->open_comm())
            {
                _currentBox = _w80.get();
                _c168->close_comm();
                continue;
            }
            break;
        }
        else
        {
            if (!_w80->open_comm())
            {
                _currentBox = _c168.get();
                _w80->close_comm();
                ec = boost::asio::error::connection_refused;
                return ec;
            }
        }
    }
    ec = boost::system::error_code();
    return ec;
}

boost::system::error_code Commbox::close(boost::system::error_code &ec)
{
    if (_currentBox == _c168.get())
    {
        if (_c168->close_comm())
        {
            ec = boost::system::error_code();
            return ec;
        }
    }
    else
    {
        if (_w80->close_comm())
        {
            ec = boost::system::error_code();
            return ec;
        }
    }
    ec = boost::asio::error::operation_not_supported;
    return ec;
}

boost::system::error_code Commbox::set_connector(Connector cnn, boost::system::error_code &ec)
{
    _shared->cnn = cnn;
    ec = boost::system::error_code();
    return ec;
}

void * Commbox::make_protocol(ProtocolType type)
{
    if (_currentBox == _c168.get())
    {
        return make_protocol<C168::Box>(type, _c168);
    }
    else
    {
        return make_protocol<w80::Box>(type, _w80);
    }
}

}
}
}
