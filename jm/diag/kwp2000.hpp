#ifndef __JM_DIAG_KWP2000_HPP__
#define __JM_DIAG_KWP2000_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/protocol.hpp>

namespace JM
{
namespace Diag
{

class KWP2000 : public Protocol
{
public:
    enum Mode
    {
        Mode8X = 0,
        Mode80 = 1,
        ModeXX = 2,
        Mode00 = 3,
        ModeCX = 4
    };

    static const boost::uint8_t KWP8X_HEADER_LENGTH = 3;
    static const boost::uint8_t KWPCX_HEADER_LENGTH = 3;
    static const boost::uint8_t KWP80_HEADER_LENGTH = 4;
    static const boost::uint8_t KWPXX_HEADER_LENGTH = 1;
    static const boost::uint8_t KWP00_HEADER_LENGTH = 2;
    static const boost::uint8_t KWP_CHECKSUM_LENGTH = 1;
    static const boost::uint16_t KWP_MAX_DATA_LENGTH = 128;
public:
    KWP2000();

    virtual JMCORE_API void setOptions(Mode msgMode, Mode linkMode,
                                       boost::int32_t baud,
                                       boost::system::error_code &ec);
    virtual JMCORE_API void setAddress(boost::uint8_t targetAddress,
                                       boost::uint8_t sourceAddress,
                                       boost::system::error_code &ec);
    virtual std::size_t pack(const boost::uint8_t* src,
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
    virtual JMCORE_API void fastInit(const boost::uint8_t *data,
                                     std::size_t count,
                                     boost::system::error_code &ec) = 0;
    virtual JMCORE_API void setLines(boost::int32_t comLine,
                                     bool lLine,
                                     boost::system::error_code &ec) = 0;
protected:
    Mode _mode;
    Mode _linkMode;
    Mode _msgMode;
    boost::int32_t _baud;
    boost::uint32_t _targetAddress;
    boost::uint32_t _sourceAddress;
};

}
}

#endif