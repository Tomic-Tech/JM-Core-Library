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
#include <jm_types.hpp>

#ifdef JM_OS_WIN
#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include <setupapi.h>
#include <dbt.h>

#elif defined JM_OS_LINUX

#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>

#endif


namespace jm {
class serial_port {
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
    
    int32 set_baudrate(int32 baudrate);
    int32 baudrate() const;
    
    int32 set_databits(uint8 databits);
    uint8 databits() const;
    
    int32 set_parity(int32 parity);
    int32 parity() const;
    
    int32 set_stopbits(int32 stopbits);
    int32 stopbits() const;
    
    int32 set_flow_control(int32 flow_control);
    int32 flow_control() const;
    
    int64 write(const uint8 *data, int64 offset, int64 count);
    int64 read(uint8 *data, int64 offset, int64 count);
    
    int32 set_read_timeout(int64 millic_seconds);
    int64 read_timeout() const;
    
    int32 set_write_timeout(int64 millic_seconds);
    int64 write_timeout() const;
    
    bool is_open();
    int32 open();
    int32 close();
    int32 flush();
    int32 discard_in_buffer();
    int32 discard_out_buffer();
    int64 bytes_available();
    int32 set_dtr(bool set);
    int32 set_rts(bool set);
    
    static std::vector<std::string> get_system_ports();
    
private:
    serial_port(const serial_port &other) {}
    serial_port& operator=(const serial_port &other) { return *this;}
    
    void initialize();
    void platform_initialize();
    void platform_destroy();
    
private:
    // Data
    std::string _port_name;
    int64 _read_timeout;
    int64 _write_timeout;
    int32 _parity;
    int32 _stopbits;
    int32 _flow_control;
    int32 _baudrate;
    bool _is_multi_setting;
    uint8 _databits;
    
#ifdef JM_OS_WIN
    int64 _bytes_to_write;
    HANDLE _handle;
    OVERLAPPED _read_overlap;
    OVERLAPPED _write_overlap;
    COMMCONFIG _comm_config;
    COMMTIMEOUTS _comm_timeouts;
    DWORD _event_mask;
    
    int32 full_name_win(const std::string &name);
    static std::vector<std::string> enumerate_device_win(const GUID *guid);
    static std::string get_reg_key_value(HKEY key, const std::string &property);
#else
    int _fd;
    struct termios _comm_config;
    struct termios _old_termios;
    struct timeval _timeout;
    struct timeval _copy_timeout;
    
    int32 set_baudrate_p(tcflag_t baud);
    int32 set_databits_p(tcflag_t bits);
#endif
};
}

#endif	/* JM_SERIAL_PORT_HPP */

