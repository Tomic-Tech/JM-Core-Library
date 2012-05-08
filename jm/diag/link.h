#ifndef __JM_DIAG_LINK_H__
#define __JM_DIAG_LINK_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace Diag
{

class Link : public boost::noncopyable
{
public:
    virtual JMCORE_API std::size_t
    send_one_frame(const boost::uint8_t *data,
                   std::size_t count,
                   boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t
    send_frames(const boost::uint8_t *data,
                std::size_t count,
                boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t
    read_one_frame(boost::uint8_t *data,
                   std::size_t max,
                   boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t
    read_frames(boost::uint8_t *data,
                std::size_t max,
                boost::system::error_code &ec) = 0;
    virtual JMCORE_API std::size_t
    send_and_recv(const boost::uint8_t *send,
                  std::size_t count,
                  boost::uint8_t *recv,
                  std::size_t max,
                  boost::system::error_code &ec) = 0;
    virtual JMCORE_API boost::system::error_code
    start_keep_link(bool run,
                    boost::system::error_code &ec) = 0;
    virtual JMCORE_API boost::system::error_code
    set_keep_link(const boost::uint8_t *data,
                  std::size_t count,
                  boost::system::error_code &ec) = 0;
    virtual JMCORE_API boost::system::error_code
    set_timeout(boost::int64_t txB2B,
                boost::int64_t rxB2B,
                boost::int64_t txF2F,
                boost::int64_t rxF2F,
                boost::int64_t total,
                boost::system::error_code &ec) = 0;

    template<typename ConstBufferSequence>
    std::size_t send_one_frame(const ConstBufferSequence &data, boost::system::error_code &ec)
    {
        return send_one_frame(boost::asio::buffer_cast<const boost::uint8_t*>(data),
                              boost::asio::buffer_size(data), ec);
    }

    template<typename ConstBufferSequence>
    std::size_t send_frames(const ConstBufferSequence &data, boost::system::error_code &ec)
    {
        return send_frames(boost::asio::buffer_cast<const boost::uint8_t*>(data),
                           boost::asio::buffer_size(data), ec);
    }

    template<typename MutableBufferSequence>
    std::size_t read_one_frame(const MutableBufferSequence &data, boost::system::error_code &ec)
    {
        return read_one_frame(boost::asio::buffer_cast<boost::uint8_t*>(data),
                              boost::asio::buffer_size(data), ec);
    }

    template<typename MutableBufferSequence>
    std::size_t read_frames(const MutableBufferSequence &data, boost::system::error_code &ec)
    {
        return read_frames(boost::asio::buffer_cast<boost::uint8_t*>(data),
                           boost::asio::buffer_size(data), ec);
    }

    template<typename ConstBufferSequence, typename MutableBufferSequence>
    std::size_t send_and_recv(const ConstBufferSequence &send, const MutableBufferSequence &recv, boost::system::error_code &ec)
    {
        return send_and_recv(boost::asio::buffer_cast<const boost::uint8_t*>(send),
                             boost::asio::buffer_size(send),
                             boost::asio::buffer_cast<boost::uint8_t*>(recv),
                             boost::asio::buffer_size(recv),
                             ec);
    }

    template<typename ConstBufferSequence>
    boost::system::error_code set_keep_link(const ConstBufferSequence &data, boost::system::error_code &ec)
    {
        return set_keep_link(boost::asio::buffer_cast<const boost::uint8_t*>(data),
                             boost::asio::buffer_size(data),
                             ec);
    }

    template<typename DurationType1, typename DurationType2, typename DurationType3, typename DurationType4, typename DurationType5>
    boost::system::error_code set_timeout(DurationType1 const &txB2B,
                                          DurationType2 const &rxB2B,
                                          DurationType3 const &txF2F,
                                          DurationType4 const &rxF2F,
                                          DurationType5 const &total,
                                          boost::system::error_code &ec)
    {
        return set_timeout(txB2B.total_microseconds(),
                           rxB2B.total_microseconds(),
                           txF2F.total_microseconds(),
                           rxF2F.total_microseconds(),
                           total.total_microseconds(),
                           ec);
    }
};

typedef boost::shared_ptr<Link> LinkPtr;

}
}

#endif /* __JM_DIAG_LINK_H__ */