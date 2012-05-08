#include "kwp1281.h"
#include "../utils.h"

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

boost::uint8_t KWP1281::frame_counter_increment()
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
    tar[0] = low_byte(srcLength + 20);
    tar[1] = frame_counter_increment();

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

boost::system::error_code
KWP1281::config(const Options &opts, boost::system::error_code &ec)
{
    memcpy(&_opts, &opts, sizeof (Options));
    ec = boost::system::error_code();
    return ec;
}

}
}
