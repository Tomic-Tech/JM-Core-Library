#ifndef __JM_DIAG_STREAM_HPP__
#define __JM_DIAG_STREAM_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <deque>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <jm/types.hpp>

namespace JM
{
namespace Diag
{

class Stream : public boost::noncopyable
{
public:
    std::size_t bytesAvailable();

    template<typename MutableBufferSequence>
    std::size_t read(const MutableBufferSequence &data,
                     boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);

        if (_deque.empty())
        {
            ec = boost::asio::error::no_data;
            return 0;
        }

        boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);
        std::size_t dataCount = boost::asio::buffer_size(data);
        std::size_t count = dataCount > _deque.size() ? _deque.size() : dataCount;

        for (std::size_t i = 0; i < count; ++i)
        {
            p[i] = _deque.front();
            _deque.pop_front();
        }
        return count;
    }

    template<typename ConstBufferSequence>
    std::size_t write(const ConstBufferSequence &data,
                      boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);

        std::size_t count = boost::asio::buffer_size(data);

        if (count <= 0)
        {
            ec = boost::asio::error::no_data;
            return 0;
        }

        const boost::uint8_t *p = boost::asio::buffer_cast<const boost::uint8_t *>(data);

        for (std::size_t i = 0; i < count; ++i)
        {
            _deque.push_back(p[i]);
        }

        return count;
    }

    void clear();
private:
    std::deque<boost::uint8_t> _deque;
    boost::mutex _mutex;
};

}
}

#endif