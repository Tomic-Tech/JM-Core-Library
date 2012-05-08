#ifndef __JM_DIAG_VIRTUAL_STREAM_H__
#define __JM_DIAG_VIRTUAL_STREAM_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/stream.h>

namespace JM
{
namespace Diag
{

class VirtualStream : public boost::noncopyable
{
public:
    VirtualStream(Stream &toCommbox,
                   Stream &fromCommbox,
                   boost::mutex &mutex,
                   boost::condition_variable &cond);

    template<typename DurationType>
    void set_read_timeout(DurationType const &time)
    {
        _readTimeout = time;
    }

    template<typename DurationType>
    void set_write_timeout(DurationType const &time)
    {
        _writeTimeout = time;
    }

    template<typename MutableBufferSequence>
    std::size_t read(const MutableBufferSequence &data,
                     boost::system::error_code &ec)
    {
        std::size_t count = boost::asio::buffer_size(data);
        if (count <= _fromCommbox.bytes_available())
            return read_immediately(data, ec);

        if (_readTimeout.total_microseconds() <= 0)
            return read_without_timeout(data, ec);

        return read_with_timeout(data, ec);
    }

    template<typename ConstBufferSequence>
    std::size_t write(const ConstBufferSequence &data,
                      boost::system::error_code &ec)
    {
        return _toCommbox.write(data, ec);
    }

    std::size_t bytes_available();
    void discard_out_buffer();
    void discard_in_buffer();

private:

    template<typename MutableBufferSequence>
    std::size_t read_immediately(const MutableBufferSequence &data,
                                boost::system::error_code &ec)
    {
        return _fromCommbox.read(data, ec);
    }

    template<typename MutableBufferSequence>
    std::size_t read_without_timeout(const MutableBufferSequence &data,
                                   boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);

        std::size_t count = boost::asio::buffer_size(data);
        boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);

        while (_fromCommbox.bytes_available() < count)
        {
            _cond.wait(lock);
        }

        return read_immediately(data, ec);
    }

    template<typename MutableBufferSequence>
    std::size_t read_with_timeout(const MutableBufferSequence &data,
                                boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);
        std::size_t count = boost::asio::buffer_size(data);
        boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);

        size_t len = 0;

        ec = boost::system::error_code();
        while (_cond.timed_wait(lock, _readTimeout))
        {
            std::size_t size = _fromCommbox.bytes_available();

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

typedef boost::shared_ptr<VirtualStream> VirtualStreamPtr;

}
}

#endif /* __JM_DIAG_VIRTUALSTREAM_H__ */