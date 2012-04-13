#ifndef __JM_DIAG_CANBUS_HPP__
#define __JM_DIAG_CANBUS_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <boost/array.hpp>
#include <jm/diag/protocol.hpp>

namespace JM
{
namespace Diag
{
class Canbus : public Protocol
{
public:
    enum Baud
    {
        B1000K = 1000000,
        B800K = 800000,
        B666K = 666000,
        B500K = 500000,
        B400K = 400000,
        B250K = 250000,
        B200K = 200000,
        B125K = 125000,
        B100K = 100000,
        B80K = 80000,
        B50K = 50000,
        B40K = 40000,
        B20K = 20000,
        B10K = 10000,
        B5K = 5000
    };

    enum FilterMask
    {
        EnableFilterMask = 0x0F,
        DisableFilterMask = 0x00
    };

    enum FrameType
    {
        DataFrame = 0x00,
        RemoteFrame = 0x40
    };

    enum IDMode
    {
        StdID = 0x00,
        ExtID = 0x80
    };

public:
    Canbus();
    virtual ~Canbus();

    virtual std::size_t pack(const boost::uint8_t *src,
                             std::size_t srcLength,
                             boost::uint8_t *tar,
                             std::size_t tarMaxLength,
                             boost::system::error_code &ec);
    virtual std::size_t unpack(const boost::uint8_t *src,
                               std::size_t srcLength,
                               boost::uint8_t *tar,
                               std::size_t tarMaxLength,
                               boost::system::error_code &ec);
    virtual JMCORE_API void init(boost::system::error_code &ec) = 0;
    virtual JMCORE_API void setLines(boost::int32_t high,
                                     boost::int32_t low,
                                     boost::system::error_code &ec);
    virtual JMCORE_API void setFilter(const std::vector<boost::int32_t> &idVector,
                                      boost::system::error_code &ec);
    virtual JMCORE_API void setOptions(boost::int32_t id, Baud baud,
                                       IDMode idMode, FilterMask mask,
                                       FrameType frame,
                                       boost::system::error_code &ec);
protected:
    boost::int32_t _targetID;
    Baud _baud;
    IDMode _idMode;
    FilterMask _filterMask;
    FrameType _frameType;
    boost::int32_t _high;
    boost::int32_t _low;
    std::vector<boost::int32_t> _idVector;
    boost::array<boost::uint8_t, 8> _flowControl;
};
}
}

#endif