#include "canbus.h"
#include "../utils.h"

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

boost::system::error_code Canbus::config(const Canbus::Options &opts, boost::system::error_code &ec)
{
    _opts.baud = opts.baud;
    _opts.filterMask = opts.filterMask;
    _opts.frameType = opts.frameType;
    _opts.highPin = opts.highPin;
    _opts.idMode = opts.idMode;
    _opts.idVector = opts.idVector;
    _opts.lowPin = opts.lowPin;
    _opts.targetID = opts.targetID;
    ec = boost::system::error_code();
    return ec;
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

    if (_opts.idMode == StdID)
    {
        tar[1] = high_byte(low_word(_opts.targetID));
        tar[2] = low_byte(low_word(_opts.targetID));
        if (_opts.frameType == DataFrame)
        {
            tar[0] = low_byte(srcLength | StdID | DataFrame);
        }
        else
        {
            tar[0] = low_byte(srcLength | StdID | RemoteFrame);
        }
        memcpy(tar + 3, src, srcLength);
        return srcLength + 3;
    }
    if (_opts.idMode == ExtID)
    {
        tar[1] = high_byte(high_word(_opts.targetID));
        tar[2] = low_byte(high_word(_opts.targetID));
        tar[3] = high_byte(low_word(_opts.targetID));
        tar[4] = low_byte(low_word(_opts.targetID));
        if (_opts.frameType == DataFrame)
        {
            tar[0] = low_byte(srcLength | ExtID | DataFrame);
        }
        else
        {
            tar[0] = low_byte(srcLength | ExtID | RemoteFrame);
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

}
}
