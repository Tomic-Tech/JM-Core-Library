#ifndef __JM_DIAG_MIKUNI_HPP__
#define __JM_DIAG_MIKUNI_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/protocol.hpp>

namespace JM
{
namespace Diag
{
class Mikuni : public Protocol
{
public:
    static const boost::uint8_t HEAD_FORMAT = 0x48;
public:
    Mikuni();
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

    virtual void init(boost::system::error_code &ec) = 0;
};
}
}

#endif