/* 
 * File:   jm_serial_port.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月15日, 下午10:40
 */

#ifndef JM_SERIAL_PORT_HPP
#define	JM_SERIAL_PORT_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <vector>
#include <jm/jm_types.hpp>
#include <jm/jm_error.hpp>

namespace jm {

class serial_port_private;
class JMCORE_API serial_port {
public:
    static const int32 parity_none = 0;
    static const int32 parity_odd = 1;
    static const int32 parity_even = 2;
    static const int32 parity_mark = 3; // Windows only
    static const int32 parity_space = 4;
    
    static const int32 stopbits_one = 0;
    static const int32 stopbits_one_point_five = 1; // Windows only
    static const int32 stopbits_two = 2;
    
    static const int32 flow_control_none = 0;
    static const int32 flow_control_hardware = 1;
    static const int32 flow_control_software = 2;
    
    serial_port();
    serial_port(const std::string &name);
    ~serial_port();
    std::string port_name() const;
    int32 set_port_name(const std::string &name);
    
    error_code set_baudrate(int32 baudrate);
    int32 baudrate() const;
    
    error_code set_databits(uint8 databits);
    uint8 databits() const;
    
    error_code set_parity(int32 parity);
    int32 parity() const;
    
    error_code set_stopbits(int32 stopbits);
    int32 stopbits() const;
    
    error_code set_flow_control(int32 flow_control);
    int32 flow_control() const;
    
    size_type write(const uint8 *data, size_type offset, size_type count);
    size_type read(uint8 *data, size_type offset, size_type count);
    
    error_code set_read_timeout(int64 millic_seconds);
    int64 read_timeout() const;
    
    error_code set_write_timeout(int64 millic_seconds);
    int64 write_timeout() const;
    
    bool is_open();
    error_code open();
    error_code close();
    error_code flush();
    error_code discard_in_buffer();
    error_code discard_out_buffer();
    size_type bytes_available();
    error_code set_dtr(bool set);
    error_code set_rts(bool set);
    
    static std::vector<std::string> get_system_ports();
    
private:
    serial_port(const serial_port &other) {}
    serial_port& operator=(const serial_port &other) { return *this;}
    
    void initialize();
    void platform_initialize();
    void platform_destroy();
    
private:
	serial_port_private *_pri;
};
}

#endif	/* JM_SERIAL_PORT_HPP */

