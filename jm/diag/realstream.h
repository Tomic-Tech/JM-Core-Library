#ifndef __JM_DIAG_REAL_STREAM_H__
#define __JM_DIAG_REAL_STREAM_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/stream.h>

namespace JM
{
namespace Diag
{

template<typename RealType>
class RealStream : public boost::noncopyable
{
public:

    RealStream(Stream &toCommbox,
                Stream &fromCommbox,
                boost::mutex &mutex,
                boost::condition_variable &cond)
    : _io()
    , _real(new RealType(_io))
    , _writeTime(_io, boost::posix_time::milliseconds(1))
    , _readTime(_io, boost::posix_time::milliseconds(1))
    , _writing(true)
    , _reading(true)
    , _toCommbox(toCommbox)
    , _fromCommbox(fromCommbox)
    , _mutex(mutex)
    , _cond(cond)
    , _writeThread()
    , _readThread()
    , _inter(boost::posix_time::milliseconds(1))
    {

    }

    ~RealStream()
    {
        stop();
    }

    boost::shared_ptr<RealType> real()
    {
        return _real;
    }

    void start()
    {
        _writeThread = boost::thread(boost::bind(&RealStream::write_to_commbox, this));
        _readThread = boost::thread(boost::bind(&RealStream::read_from_commbox, this));
    }

    void stop()
    {
        _writing = false;
        _reading = false;
        _writeThread.join();
        _readThread.join();
    }

private:

    void write_to_commbox()
    {
        StaticBytes cache;
        boost::system::error_code ec;
        _writing = true;
        while (_writing)
        {
            std::size_t avail = _toCommbox.bytes_available();
            if (!avail)
            {
                boost::this_thread::sleep(_inter);
                boost::this_thread::yield();
                continue;
            }
            _toCommbox.read(boost::asio::mutable_buffer(cache.data(), avail), ec);
            boost::asio::write(*_real, boost::asio::const_buffers_1(cache.data(), avail), ec);
            _writeTime.expires_from_now(_inter, ec);
            _writeTime.wait(ec);
            _io.run(ec);
        }
    }

    void read_from_commbox()
    {
        boost::array<boost::uint8_t, 255 > cache;
        boost::system::error_code ec;
        _reading = true;
        while (_reading)
        {
            std::size_t avail = _real->bytes_available(ec);
            boost::asio::read(*_real, boost::asio::mutable_buffers_1(cache.data(), avail), ec);
            if (ec || !avail)
            {
                boost::this_thread::sleep(_inter);
                boost::this_thread::yield();
                continue;
            }
            _io.run(ec);
            boost::unique_lock<boost::mutex> lock(_mutex);
            _fromCommbox.write(boost::asio::const_buffer(cache.data(), avail), ec);
            _cond.notify_one();
        }
    }
private:
    boost::asio::io_service _io;
    boost::asio::deadline_timer _writeTime;
    boost::asio::deadline_timer _readTime;
    bool _writing;
    bool _reading;
    boost::shared_ptr<RealType> _real;
    Stream &_toCommbox;
    Stream &_fromCommbox;
    boost::uint8_t _fromCommboxBuff;
    boost::mutex &_mutex;
    boost::condition_variable &_cond;
    boost::thread _writeThread;
    boost::thread _readThread;
    boost::posix_time::time_duration _inter;
};

}
}


#endif /* __JM_DIAG_REALSTREAM_H__ */

