#include "kwp1281.hpp"
#include <jm/utils.hpp>

namespace JM
{
namespace Diag
{
KWP1281::KWP1281()
    : _frameCounter(0)
{

}

KWP1281::~KWP1281()
{

}

boost::uint8_t KWP1281::frameCounterIncrement()
{
    return ++(_frameCounter);
}

std::size_t KWP1281::pack(const boost::uint8_t *src,
                          std::size_t srcLength,
                          boost::uint8_t *tar,
                          std::size_t tarLength,
                          boost::system::error_code &ec)
{
    ec = boost::system::error_code();
    tar[0] = LowByte(srcLength + 20);
    tar[1] = frameCounterIncrement();

    memcpy(tar + 2, src, srcLength);
    tar[srcLength + 2] = FRAME_END;
    return srcLength + 3;
}

std::size_t KWP1281::unpack(const boost::uint8_t *src,
                            std::size_t srcLength,
                            boost::uint8_t *tar,
                            std::size_t tarLength,
                            boost::system::error_code &ec)
{
    ec = boost::system::error_code();
    memcpy(tar, src + 1, srcLength - 2);
    return srcLength - 2;
}
}
}