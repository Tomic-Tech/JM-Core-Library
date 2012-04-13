#include "mikuni.hpp"

namespace JM
{
namespace Diag
{
Mikuni::Mikuni()
{

}

std::size_t Mikuni::pack(const boost::uint8_t *src,
                         std::size_t srcLength,
                         boost::uint8_t *tar,
                         std::size_t tarLength,
                         boost::system::error_code &ec)
{
    if (srcLength <= 0)
    {
        ec = boost::asio::error::message_size;
        return 0;
    }

    if ((srcLength + 3) > tarLength)
    {
        ec = boost::asio::error::message_size;
        return 0;
    }

    tar[0] = HEAD_FORMAT;
    memcpy(tar + 1, src, srcLength);
    tar[srcLength + 1] = 0x0D;
    tar[srcLength + 2] = 0x0A;

    ec = boost::system::error_code();
    return srcLength + 3;
}

std::size_t Mikuni::unpack(const boost::uint8_t *src,
                           std::size_t srcLength,
                           boost::uint8_t *tar,
                           std::size_t tarLength,
                           boost::system::error_code &ec)
{
    if (srcLength <= 0)
    {
        ec = boost::asio::error::message_size;
        return 0;
    }

    if ((srcLength - 3) > tarLength)
    {
        ec = boost::asio::error::message_size;
        return 0;
    }

    memcpy(tar, src + 1, srcLength - 3);

    ec = boost::system::error_code();

    return srcLength - 3;
}
}
}