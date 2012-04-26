#include "kwp2000.hpp"
#include <jm/utils.hpp>

namespace JM
{
namespace Diag
{
KWP2000::KWP2000()
    : _mode(Mode8X)
    , _linkMode(Mode8X)
    , _msgMode(Mode8X)
    , _baud(10416)
    , _targetAddress(0x12)
    , _sourceAddress(0xF1)
{

}

void KWP2000::setOptions(Mode msgMode, Mode linkMode, boost::int32_t baud,
                         boost::system::error_code &ec)
{
    _msgMode = msgMode;
    _linkMode = linkMode;
    _mode = msgMode;
    _baud = baud;

    ec = boost::system::error_code();
}

void KWP2000::setAddress(boost::uint8_t targetAddress,
                         boost::uint8_t sourceAddress,
                         boost::system::error_code &ec)
{
    _targetAddress = targetAddress;
    _sourceAddress = sourceAddress;

    ec = boost::system::error_code();
}

std::size_t KWP2000::pack(const boost::uint8_t* src,
                          std::size_t srcLength,
                          boost::uint8_t *tar,
                          std::size_t tarLength,
                          boost::system::error_code &ec)
{
    std::size_t pos = 0;
    boost::uint8_t checksum = 0;
    std::size_t i = 0;

    if (_mode == Mode8X)
    {
        tar[pos++] = LowByte(0x80 | srcLength);
        tar[pos++] = LowByte(_targetAddress);
        tar[pos++] = LowByte(_sourceAddress);
        memcpy(tar + pos, src, srcLength);
        pos += srcLength;
    }
    else if (_mode == ModeCX)
    {
        tar[pos++] = LowByte(0xC0 | srcLength);
        tar[pos++] = LowByte(_targetAddress);
        tar[pos++] = LowByte(_sourceAddress);
        memcpy(tar + pos, src, srcLength);
        pos += srcLength;
    }
    else if (_mode == Mode80)
    {
        tar[pos++] = LowByte(0x80);
        tar[pos++] = LowByte(_targetAddress);
        tar[pos++] = LowByte(_sourceAddress);
        tar[pos++] = LowByte(srcLength);
        memcpy(tar + pos, src, srcLength);
        pos += srcLength;
    }
    else if (_mode == Mode00)
    {
        tar[pos++] = 0x00;
        tar[pos++] = LowByte(srcLength);
        memcpy(tar + pos, src, srcLength);
        pos += srcLength;
    }
    else if (_mode == ModeXX)
    {
        tar[pos++] = LowByte(srcLength);
        memcpy(tar + pos, src, srcLength);
        pos += srcLength;
    }
    else
    {
        ec = boost::system::error_code(boost::system::errc::protocol_error,
                                       boost::system::get_system_category());
    }

    for (i = 0; i < pos; i++)
    {
        checksum += tar[i];
    }
    tar[pos++] = checksum;
    return pos;
}

std::size_t KWP2000::unpack(const boost::uint8_t *src,
                            std::size_t srcLength,
                            boost::uint8_t *tar,
                            std::size_t tarLength,
                            boost::system::error_code &ec)
{
    std::size_t length = 0;

    if (src[0] > 0x80)
    {
        length = src[0] - 0x80;
        if (src[1] != _sourceAddress)
        {
            ec = boost::system::error_code(boost::system::errc::bad_message,
                                           boost::system::get_system_category());
            return 0;
        }
        if (length != (srcLength - KWP8X_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
        {
            length = src[0] - 0xC0; /* For KWPCX */
            if (length != (srcLength - KWPCX_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
            {
                ec = boost::system::error_code(boost::system::errc::message_size,
                                               boost::system::get_system_category());
                return 0;
            }
        }
        memcpy(tar, src + KWP8X_HEADER_LENGTH, length);
    }
    else if (src[0] == 0x80)
    {
        length = src[0];
        if (src[1] != _sourceAddress)
        {
            ec = boost::system::error_code(boost::system::errc::bad_message,
                                           boost::system::get_system_category());
            return 0;
        }

        if (length != (srcLength - KWP80_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
        {
            ec = boost::system::error_code(boost::system::errc::message_size,
                                           boost::system::get_system_category());
            return 0;
        }
        memcpy(tar, src + KWP80_HEADER_LENGTH, length);
    }
    else if (src[0] == 0x00)
    {
        length = src[1];
        if (length != (srcLength - KWP00_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
        {
            ec = boost::system::error_code(boost::system::errc::message_size,
                                           boost::system::get_system_category());
            return 0;
        }
        memcpy(tar, src + KWP00_HEADER_LENGTH, length);
    }
    else
    {
        length = src[0];
        if (length != (srcLength - KWP00_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
        {
            ec = boost::system::error_code(boost::system::errc::message_size,
                                           boost::system::get_system_category());
            return 0;
        }
        memcpy(tar, src + KWP00_HEADER_LENGTH, length);
    }
    return length;
}
}
}