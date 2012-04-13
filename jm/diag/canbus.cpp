#include "canbus.hpp"
#include <jm/utils.hpp>

namespace JM
{
namespace Diag
{
Canbus::Canbus()
{
    _flowControl[0] = 0x30;
    _flowControl[1] = 0x00;
    _flowControl[2] = 0x00;
    _flowControl[3] = 0x00;
    _flowControl[4] = 0x00;
    _flowControl[5] = 0x00;
    _flowControl[6] = 0x00;
    _flowControl[7] = 0x00;
}

Canbus::~Canbus()
{

}

std::size_t Canbus::pack(const boost::uint8_t *src,
                         std::size_t srcLength,
                         boost::uint8_t *tar,
                         std::size_t tarMaxLength,
                         boost::system::error_code &ec)
{
    if (srcLength > 8 || srcLength <= 0)
    {
        ec = boost::asio::error::message_size;
        return 0;
    }

    if (_idMode == StdID)
    {
        tar[1] = HighByte(LowWord(_targetID));
        tar[2] = LowByte(LowWord(_targetID));
        if (_frameType == DataFrame)
        {
            tar[0] = LowByte(srcLength | StdID | DataFrame);
        }
        else
        {
            tar[0] = LowByte(srcLength | StdID | RemoteFrame);
        }
        memcpy(tar + 3, src, srcLength);
        return srcLength + 3;
    }
    if (_idMode == ExtID)
    {
        tar[1] = HighByte(HighWord(_targetID));
        tar[2] = LowByte(HighWord(_targetID));
        tar[3] = HighByte(LowWord(_targetID));
        tar[4] = LowByte(LowWord(_targetID));
        if (_frameType == DataFrame)
        {
            tar[0] = LowByte(srcLength | ExtID | DataFrame);
        }
        else
        {
            tar[0] = LowByte(srcLength | ExtID | RemoteFrame);
        }
        memcpy(tar + 5, src, srcLength);
        return srcLength + 5;
    }
    return 0;
}

std::size_t Canbus::unpack(const boost::uint8_t *src,
                           std::size_t srcLength,
                           boost::uint8_t *tar,
                           std::size_t tarMaxLength,
                           boost::system::error_code &ec)
{
    if (srcLength <= 0)
    {
        ec = boost::asio::error::message_size;
        return 0;
    }

    std::size_t length = 0;

    boost::uint32_t mode = (src[0] & (ExtID | RemoteFrame));
    if (mode == (StdID | DataFrame))
    {
        length = src[0] & 0x0F;
        if (length != srcLength - 3)
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        memcpy(tar, src + 3, length);
    }

    if (mode == (ExtID | DataFrame))
    {
        length = src[0] & 0x0F;
        if (length != srcLength - 5)
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        memcpy(tar, src + 5, length);
    }
    return length;
}

void Canbus::setLines(boost::int32_t high, boost::int32_t low,
                      boost::system::error_code &ec)
{
    _high = high;
    _low = low;

    ec = boost::system::error_code();
}

void Canbus::setFilter(const std::vector<boost::int32_t> &idVector,
                       boost::system::error_code &ec)
{
    _idVector = idVector;
    ec = boost::system::error_code();
}

void Canbus::setOptions(boost::int32_t id, Baud baud, IDMode idMode,
                        FilterMask mask, FrameType frame, boost::system::error_code &ec)
{
    _baud = baud;
    _idMode = idMode;
    _filterMask = mask;
    _frameType = frame;
    _targetID = id;
    ec = boost::system::error_code();
}

}
}