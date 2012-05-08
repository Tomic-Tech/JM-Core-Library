#ifndef __JM_DIAG_MIKUNI_H__
#define __JM_DIAG_MIKUNI_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/protocol.h>

namespace JM
{
namespace Diag
{

class Mikuni : public Protocol
{
public:
    static const boost::uint8_t HEAD_FORMAT = 0x48;
protected:
public:
    Mikuni();
    ~Mikuni();

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
    config(boost::system::error_code &ec) = 0;

};

typedef boost::shared_ptr<Mikuni> MikuniPtr;

}
}

#endif /* __JM_DIAG_MIKUNI_H__ */