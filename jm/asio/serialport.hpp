#ifndef __JM_ASIO_SERIAL_PORT_HPP__
#define __JM_ASIO_SERIAL_PORT_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/config.hpp>
#include <boost/asio.hpp>
#include <jm/types.hpp>

#ifdef BOOST_WINDOWS
#include <Windows.h>
#endif

namespace JM
{
namespace Asio
{
class SerialPort : public boost::noncopyable
{
public:
    static StringVector getSystemPorts();

#ifdef BOOST_WINDOWS
    static void setDtr(HANDLE handle, bool set,
                       boost::system::error_code &ec);
    static std::size_t bytesAvailable(HANDLE handle,
                                      boost::system::error_code &ec);
#else
	static void setDtr(int fd, bool set,
		boost::system::error_code &ec);
	static std::size_t bytesAvailable(int fd,
		boost::system::error_code &ec);
#endif
    template<typename NativeHandle>
    static void setDtr(NativeHandle handle, bool set,
                       boost::system::error_code &ec)
    {
        setDtr(handle, set, ec);
    }

    template<typename NativeHandle>
    static std::size_t bytesAvailalbe(NativeHandle handle,
                                      boost::system::error_code &ec)
    {
        return bytesAvailalbe(handle, ec);
    }
};
}
}

#endif
