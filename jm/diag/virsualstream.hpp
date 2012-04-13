#ifndef __JM_DIAG_VISUAL_STREAM_HPP__
#define __JM_DIAG_VISUAL_STREAM_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/thread.hpp>
#include <jm/diag/stream.hpp>

namespace JM
{
namespace Diag
{

class VisualStream : public boost::noncopyable
{
public:
    VisualStream(Stream &toCommbox,
                 Stream &fromCommbox,
                 boost::mutex &mutex,
                 boost::condition_variable &cond);

    template<typename DurationType>
    void setReadTimeout(DurationType const &time)
    {
        _readTimeout = time;
    }

    template<typename DurationType>
    void setWriteTimeout(DurationType const &time)
    {
        _writeTimeout = time;
    }

    std::size_t bytesAvailable();
    void discardOutBuffer();
    void discardInBuffer();

    template<typename MutableBufferSequence>
    std::size_t read(const MutableBufferSequence &data,
                     boost::system::error_code &ec)
    {
        std::size_t count = boost::asio::buffer_size(data);
        if (count <= _fromCommbox.bytesAvailable())
            return readImmediately(data, ec);

        if (_readTimeout.total_microseconds() <= 0)
            return readWithoutTimeout(data, ec);

        return readWithTimeout(data, ec);
    }

    template<typename ConstBufferSequence>
    std::size_t write(const ConstBufferSequence &data,
                      boost::system::error_code &ec)
    {
        return _toCommbox.write(data, ec);
    }
private:
    template<typename MutableBufferSequence>
    std::size_t readImmediately(const MutableBufferSequence &data,
                                boost::system::error_code &ec)
    {
        return _fromCommbox.read(data, ec);
    }

    template<typename MutableBufferSequence>
    std::size_t readWithoutTimeout(const MutableBufferSequence &data,
                                   boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);

        std::size_t count = boost::asio::buffer_size(data);
        boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);

        while(_fromCommbox.bytesAvailable() < count)
        {
            _cond.wait(lock);
        }

        return readImmediately(data, ec);
    }

    template<typename MutableBufferSequence>
    std::size_t readWithTimeout(const MutableBufferSequence &data,
                                boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);
        std::size_t count = boost::asio::buffer_size(data);
        boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);

        size_t len = 0;

        ec = boost::system::error_code();
        while (_cond.timed_wait(lock, _readTimeout))
        {
            std::size_t size = _fromCommbox.bytesAvailable();

            if (size >= count)
            {
                _fromCommbox.read(boost::asio::mutable_buffer(p + len, count), ec);
                len += count;
                break;
            }
            _fromCommbox.read(boost::asio::mutable_buffer(p + len, size), ec);
            len += size;
            count -= size;
        }

        if (len < boost::asio::buffer_size(data))
        {
            ec = boost::asio::error::message_size;
        }

        return len;
    }
private:
    Stream &_toCommbox;
    Stream &_fromCommbox;
    boost::mutex &_mutex;
    boost::condition_variable &_cond;
    boost::posix_time::time_duration _readTimeout;
    boost::posix_time::time_duration _writeTimeout;
};
}
}

#endif