#include "boxversion.hpp"

namespace JM
{
namespace Diag
{
namespace V1
{
BoxVersion::BoxVersion()
    : _shared(new Shared())
    , _w80(new W80::Box(_shared))
    , _c168(new C168::Box(_shared))
    , _currentBox(_c168.get())
    , _w80PrcHash()
    , _c168PrcHash()
{
    _shared->buffID = 0;
    _shared->connector = CN_UNKNOW;
    _shared->isDB20 = false;
    _shared->isDoNow = true;
    _shared->lastError = 0;
    _shared->nextAddress = 0;
    _shared->reqByteToByte = 500000;
    _shared->reqWaitTime = 500000;
    _shared->resByteToByte = 500000;
    _shared->resWaitTime = 500000;
}

BoxVersion::~BoxVersion()
{

}

void BoxVersion::open(boost::system::error_code &ec)
{
    while (true)
    {
        if (_currentBox == _c168.get())
        {
            if (!_c168->openComm())
            {
                _currentBox = _w80.get();
                _c168->closeComm();
                continue;
            }
            break;
        }
        else
        {
            if (!_w80->openComm())
            {
                _currentBox = _c168.get();
                _w80->closeComm();
                ec = boost::asio::error::connection_refused;
                return;
            }
        }
    }
    ec = boost::system::error_code();
}

void BoxVersion::close(boost::system::error_code &ec)
{
    if (_currentBox == _c168.get())
    {
        if (_c168->closeComm())
        {
            ec = boost::system::error_code();
        }
    }
    else
    {
        if (_w80->closeComm())
        {
            ec = boost::system::error_code();
        }
    }
    ec = boost::asio::error::operation_not_supported;
}

Diag::BoxLink* BoxVersion::configure(ProtocolType type)
{
    Diag::BoxLink *link = NULL;

    if (_currentBox == _c168.get())
    {
        if (_c168PrcHash.find(type) != _c168PrcHash.end())
        {
            link = _c168PrcHash[type].get();
        }
        if (link == NULL)
        {
            _c168PrcHash[type] = makeProtocol<C168::Box>(_c168, type);
            link = _c168PrcHash[type].get();
        }
    }
    else if (_currentBox == _w80.get())
    {
        if (_w80PrcHash.find(type) != _w80PrcHash.end())
        {
            link = _w80PrcHash[type].get();
        }

        if (link == NULL)
        {
            _w80PrcHash[type] = makeProtocol<W80::Box>(_w80, type);
            link = _w80PrcHash[type].get();
        }
    }
    return link;
}

void BoxVersion::setConnector(Connector type)
{
    _shared->connector = type;
}

}
}
}