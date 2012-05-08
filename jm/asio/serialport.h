#ifndef __JM_ASIO_SERIALPORT_H__
#define __JM_ASIO_SERIALPORT_H__

#ifndef _MSC_VER
#pragma once
#endif

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <jm/types.h>

namespace JM
{
namespace Asio
{

class serial_port : public boost::asio::serial_port
{
public:
    JMCORE_API serial_port(boost::asio::io_service &ioService);
    JMCORE_API serial_port(boost::asio::io_service &ioService, const char *device);
    JMCORE_API serial_port(boost::asio::io_service &ioService, const std::string &device);
    JMCORE_API boost::system::error_code set_dtr(bool set, boost::system::error_code &ec);
    JMCORE_API boost::system::error_code set_rts(bool set, boost::system::error_code &ec);
    JMCORE_API static StringVector get_system_ports();
    JMCORE_API std::size_t bytes_available(boost::system::error_code &ec);
    JMCORE_API boost::system::error_code discard_in_buffer(boost::system::error_code &ec);
    JMCORE_API boost::system::error_code discard_out_buffer(boost::system::error_code &ec);
};

typedef boost::shared_ptr<serial_port> serial_port_ptr;


}
}

#endif
