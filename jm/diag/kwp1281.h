#ifndef __JM_DIAG_KWP1281_H__
#define __JM_DIAG_KWP1281_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/protocol.h>

namespace JM
{
namespace Diag
{

class KWP1281 : public Protocol
{
public:
    struct Options
    {
        boost::int32_t comLine;
        bool lLine;
    };
    static const boost::uint8_t FRAME_END = 0x03;
public:
    KWP1281();
    virtual ~KWP1281();

    virtual JMCORE_API std::size_t pack(const boost::uint8_t *src,
                             std::size_t srcLength,
                             boost::uint8_t *tar,
                             std::size_t tarLength,
                             boost::system::error_code &ec);
    virtual JMCORE_API std::size_t unpack(const boost::uint8_t *src,
                               std::size_t srcLength,
                               boost::uint8_t *tar,
                               std::size_t tarLength,
                               boost::system::error_code &ec);
    virtual JMCORE_API boost::system::error_code
    addr_init(boost::uint8_t addrCode, boost::system::error_code &ec) = 0;
    
    virtual JMCORE_API boost::system::error_code
    config(const Options &opts, boost::system::error_code &ec);
protected:
private:
    boost::uint8_t frame_counter_increment();
protected:
    Options _opts;
    boost::uint8_t _frameCounter;
};

typedef boost::shared_ptr<KWP1281> KWP1281Ptr;

}
}

#endif /* __JM_DIAG_KWP1281_H__ */
