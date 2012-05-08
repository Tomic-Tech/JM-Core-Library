#ifndef __JM_DIAG_CANBUS_H__
#define __JM_DIAG_CANBUS_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/protocol.h>

namespace JM
{
namespace Diag
{

class Canbus : public Protocol
{
public:

    enum Baudrate
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

    struct Options
    {
        boost::int32_t targetID;
        Canbus::Baudrate baud;
        Canbus::IDMode idMode;
        Canbus::FilterMask filterMask;
        Canbus::FrameType frameType;
        boost::int32_t highPin;
        boost::int32_t lowPin;
        IntVectorPtr idVector;
    };

public:
    Canbus();
    virtual ~Canbus();

    virtual JMCORE_API std::size_t pack(const boost::uint8_t *src,
                             std::size_t srcLength,
                             boost::uint8_t *tar,
                             std::size_t tarMaxLength,
                             boost::system::error_code &ec);
    virtual JMCORE_API std::size_t unpack(const boost::uint8_t *src,
                               std::size_t srcLength,
                               boost::uint8_t *tar,
                               std::size_t tarMaxLength,
                               boost::system::error_code &ec);
    virtual JMCORE_API boost::system::error_code config(const Options &opts, boost::system::error_code &ec);

protected:
    boost::array<boost::uint8_t, 8 > _flowControl;
    Options _opts;
};

typedef boost::shared_ptr<Canbus> CanbusPtr;

}
}

#endif /* __JM_DIAG_CANBUS_H__ */
