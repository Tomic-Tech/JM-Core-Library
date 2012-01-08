#ifndef JM_SERIAL_PORT_PRIVATE_HPP
#define JM_SERIAL_PORT_PRIVATE_HPP

#include <jm/jm_types.hpp>

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

#include <boost/thread.hpp>

namespace jm {

class serial_port_private {
private:
	friend class serial_port;
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
    boost::recursive_mutex _mutex;
    
#ifdef JM_OS_WIN
    int64 _bytes_to_write;
    HANDLE _handle;
    OVERLAPPED _read_overlap;
    OVERLAPPED _write_overlap;
    COMMCONFIG _comm_config;
    COMMTIMEOUTS _comm_timeouts;
    DWORD _event_mask;
    
    static error_code full_name_win(const std::string &name, std::string &result);
    static std::vector<std::string> enumerate_device_win(const GUID *guid);
    static std::string get_reg_key_value(HKEY key, const std::string &property);
#else
    int _fd;
    struct termios _comm_config;
    struct termios _old_termios;
    struct timeval _timeout;
    struct timeval _copy_timeout;
    
    error_code set_baudrate_p(tcflag_t baud);
    error_code set_databits_p(tcflag_t bits);
#endif
};
}

#endif 