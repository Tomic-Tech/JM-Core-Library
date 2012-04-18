#ifndef __JM_DIAG_BOX_LINK_HPP__
#define __JM_DIAG_BOX_LINK_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <jm/types.hpp>
#include <jm/diag/canbus.hpp>
#include <jm/diag/kwp2000.hpp>
#include <jm/diag/kwp1281.hpp>
#include <jm/diag/mikuni.hpp>

namespace JM
{
namespace Diag
{
class BoxLink
{
protected:
    BoxLink();
    virtual ~BoxLink();
    void setProtocol(void *p);
public:
    template<typename P>
    P* protocol()
    {
        return static_cast<P*>(_ptr);
    }

    virtual JMCORE_API std::size_t sendOneFrame(const boost::uint8_t *data,
            std::size_t length,
            boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t sendFrames(const boost::uint8_t *data,
            std::size_t length,
            boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t readOneFrame(boost::uint8_t *data,
            std::size_t maxLength,
            boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t readFrames(boost::uint8_t *data,
            std::size_t maxLength,
            boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t sendAndRecv(const boost::uint8_t *send,
            std::size_t sendLenth,
            boost::uint8_t *recv,
            std::size_t recvMaxLength,
            boost::system::error_code &ec) = 0;
    virtual JMCORE_API void startKeepLink(bool run,
                                          boost::system::error_code &ec) = 0;
    virtual JMCORE_API void setKeepLink(const boost::uint8_t *data,
                                        std::size_t length,
                                        boost::system::error_code &ec) = 0;
    // All parameter unit is microseconds.
    virtual JMCORE_API void setTimeout(boost::int64_t txB2B,
            boost::int64_t rxB2B,
            boost::int64_t txF2F,
            boost::int64_t rxF2F,
            boost::int64_t total,
            boost::system::error_code &ec) = 0;

    template<typename ConstBufferSequence>
    std::size_t sendOneFrame(const ConstBufferSequence &data,
                             boost::system::error_code &ec)
    {
        return sendOneFrame(boost::asio::buffer_cast<const boost::uint8_t*>(data),
                            boost::asio::buffer_size(data), ec);
    }

    template<typename ConstBufferSequence>
    std::size_t sendFrames(const ConstBufferSequence &data,
                           boost::system::error_code &ec)
    {
        return sendFrames(boost::asio::buffer_cast<const boost::uint8_t*>(data),
                          boost::asio::buffer_size(data), ec);
    }

    template<typename MutableBufferSequence>
    std::size_t readOneFrame(const MutableBufferSequence &data,
                             boost::system::error_code &ec)
    {
        return readOneFrame(boost::asio::buffer_cast<boost::uint8_t *>(data),
                            boost::asio::buffer_size(data), ec);
    }

    template<typename MutableBufferSequence>
    std::size_t readFrames(const MutableBufferSequence &data,
                           boost::system::error_code &ec)
    {
        return readOneFrame(boost::asio::buffer_cast<boost::uint8_t *>(data),
                            boost::asio::buffer_size(data), ec);
    }

    template<typename ConstBufferSequence, typename MutableBufferSequence>
    std::size_t sendAndRecv(const ConstBufferSequence &send,
                            const MutableBufferSequence &recv,
                            boost::system::error_code &ec)
    {
        return sendAndRecv(boost::asio::buffer_cast<const boost::uint8_t*>(send),
                           boost::asio::buffer_size(send),
                           boost::asio::buffer_cast<boost::uint8_t*>(recv),
                           boost::asio::buffer_size(recv),
                           ec);
    }

    template<typename ConstBufferSequence>
    void setKeepLink(const ConstBufferSequence &data,
                     boost::system::error_code &ec)
    {
        return setKeepLink(boost::asio::buffer_cast<const boost::uint8_t*>(data),
                           boost::asio::buffer_size(data),
                           ec);
    }

    template<typename DurationType1, typename DurationType2, typename DurationType3, typename DurationType4, typename DurationType5>
    void setTimeout(DurationType1 const &txB2B,
                    DurationType2 const &rxB2B,
                    DurationType3 const &txF2F,
                    DurationType4 const &rxF2F,
                    DurationType5 const &total,
                    boost::system::error_code &ec)
    {
        return setTimeout(txB2B.total_microseconds(),
                          rxB2B.total_microseconds(),
                          txF2F.total_microseconds(),
                          rxF2F.total_microseconds(),
                          total.total_microseconds(),
                          ec);
    }
private:
    void *_ptr;
};
}
}

#endif