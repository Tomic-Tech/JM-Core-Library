#ifndef __JM_DIAG_KWP2000_H__
#define __JM_DIAG_KWP2000_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/protocol.h>

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

    struct Options
    {
        Mode linkMode;
        Mode msgMode;
        boost::int32_t baud;
        boost::uint32_t targetAddress;
        boost::uint32_t sourceAddress;
        boost::int32_t comLine;
        bool lLine;
    };

public:
    KWP2000();
    virtual ~KWP2000();

    virtual JMCORE_API std::size_t pack(const boost::uint8_t* src,
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
    addr_init(boost::uint8_t addrCode,
              boost::system::error_code &ec) = 0;
    virtual JMCORE_API boost::system::error_code
    fast_init(const boost::uint8_t *data,
              std::size_t count,
              boost::system::error_code &ec) = 0;

    template<typename ConstBufferSequence>
    boost::system::error_code
    fast_init(const ConstBufferSequence &buff, boost::system::error_code &ec)
    {
        const boost::uint8_t *data = boost::asio::buffer_cast<const boost::uint8_t*>(buff);
        std::size_t count = boost::asio::buffer_size(buff);
        return fast_init(data, count, ec);
    }

    virtual JMCORE_API boost::system::error_code
    config(const Options &opts, boost::system::error_code &ec);

protected:
    Options _opts;
    Mode _mode;
};

typedef boost::shared_ptr<KWP2000> KWP2000Ptr;

}
}

#endif /* __JM_DIAG_KWP2000_H__ */
