#ifndef __JM_DIAG_KWP1281_HPP__
#define __JM_DIAG_KWP1281_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/protocol.hpp>

namespace JM
{
namespace Diag
{
class KWP1281 : public Protocol
{
public:
    static const boost::uint8_t FRAME_END = 0x03;
public:
    KWP1281();
    virtual ~KWP1281();
    virtual std::size_t pack(const boost::uint8_t *src,
                             std::size_t srcLength,
                             boost::uint8_t *tar,
                             std::size_t tarLength,
                             boost::system::error_code &ec);
    virtual std::size_t unpack(const boost::uint8_t *src,
                               std::size_t srcLength,
                               boost::uint8_t *tar,
                               std::size_t tarLength,
                               boost::system::error_code &ec);

    virtual JMCORE_API void addrInit(boost::uint8_t addrCode,
                                     boost::system::error_code &ec) = 0;
    virtual JMCORE_API void setLines(boost::int32_t comLine,
                                     bool lLine,
                                     boost::system::error_code &ec) = 0;

private:
    boost::uint8_t frameCounterIncrement();

protected:
    boost::uint8_t _frameCounter;
};
}
}


#endif