#ifndef __JM_DIAG_PROTOCOL_HPP__
#define __JM_DIAG_PROTOCOL_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <jm/types.hpp>

namespace JM
{
namespace Diag
{
class Protocol : public boost::noncopyable
{
public:
    virtual std::size_t pack(const boost::uint8_t *src,
                             std::size_t srcLength,
                             boost::uint8_t *tar,
                             std::size_t tarLength,
                             boost::system::error_code &ec) = 0;
    virtual std::size_t unpack(const boost::uint8_t *src,
                               std::size_t srcLength,
                               boost::uint8_t *tar,
                               std::size_t tarLength,
                               boost::system::error_code &ec) = 0;

    template<typename ConstBufferSequence, typename MutableBufferSequence>
    std::size_t pack(const ConstBufferSequence &src,
                     const MutableBufferSequence &tar,
                     boost::system::error_code &ec)
    {
        return pack(boost::asio::buffer_cast<const boost::uint8_t*>(src),
                    boost::asio::buffer_size(src),
                    boost::asio::buffer_cast<boost::uint8_t*>(tar),
                    boost::asio::buffer_size(tar),
                    ec);
    }

    template<typename ConstBufferSequence, typename MutableBufferSequence>
    std::size_t unpack(const ConstBufferSequence &src,
                       const MutableBufferSequence &tar,
                       boost::system::error_code &ec)
    {
        return unpack(boost::asio::buffer_cast<const boost::uint8_t*>(src),
                      boost::asio::buffer_size(src),
                      boost::asio::buffer_cast<boost::uint8_t*>(tar),
                      boost::asio::buffer_size(tar),
                      ec);
    }
};
}
}

#endif