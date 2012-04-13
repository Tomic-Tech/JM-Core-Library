#ifndef __JM_DIAG_DEVICE_STREAM_HPP__
#define __JM_DIAG_DEVICE_STREAM_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <jm/diag/stream.hpp>
#include <jm/asio/serialport.hpp>

namespace JM
{
namespace Diag
{
template<typename PhysicalType>
class PhysicalStream : public boost::noncopyable
{
public:
    PhysicalStream(Stream &toCommbox,
                   Stream &fromCommbox,
                   boost::mutex &mutex,
                   boost::condition_variable &cond)
        : _io()
        , _physical(_io)
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
        _writeThread = boost::thread(boost::bind(&PhysicalStream::writeToCommbox, this));
        _readThread = boost::thread(boost::bind(&PhysicalStream::readFromCommbox, this));
    }

    PhysicalType& physical()
    {
        return _physical;
    }
private:

    void writeToCommbox()
    {
        boost::array<boost::uint8_t, 255> cache;
        boost::system::error_code ec;
        _writing = true;
        while (_writing)
        {
            std::size_t avail = _toCommbox.bytesAvailable();
            if (!avail)
            {
                boost::this_thread::sleep(_inter);
                boost::this_thread::yield();
                continue;
            }
            _toCommbox.read(boost::asio::mutable_buffer(cache.data(), avail), ec);
            boost::asio::write(_physical, boost::asio::const_buffers_1(cache.data(), avail), ec);
            _writeTime.expires_from_now(_inter, ec);
            _writeTime.wait(ec);
            _io.run(ec);
        }
    }

    void readFromCommbox()
    {
        boost::array<boost::uint8_t, 255> cache;
        boost::system::error_code ec;
        _reading = true;
        while (_reading)
        {
            std::size_t avail = JM::Asio::SerialPort::bytesAvailable(_physical.native_handle(), ec);
            boost::asio::read(_physical, boost::asio::mutable_buffers_1(cache.data(), avail), ec);
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
    PhysicalType _physical;
    Stream &_toCommbox;
    Stream &_fromCommbox;
    boost::uint8_t fromCommboxBuff;
    boost::mutex &_mutex;
    boost::condition_variable &_cond;
    boost::thread _writeThread;
    boost::thread _readThread;
    boost::posix_time::time_duration _inter;
};

}
}

#endif