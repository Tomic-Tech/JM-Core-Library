#include "serialport.h"

namespace JM
{
namespace Asio
{

serial_port::serial_port(boost::asio::io_service &io_service)
: boost::asio::serial_port(io_service)
{

}

serial_port::serial_port(boost::asio::io_service &io_service, const char *device)
: boost::asio::serial_port(io_service, device)
{

}

serial_port::serial_port(boost::asio::io_service &io_service, const std::string &device)
: boost::asio::serial_port(io_service, device)
{

}

#ifdef BOOST_WINDOWS

#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include <setupapi.h>
#include <dbt.h>
#include <boost/asio/error.hpp>
#include <boost/system/error_code.hpp>
#include <winbase.h>

// see http://msdn.microsoft.com/en-us/library/ms791134.aspx for list of GUID classes
#ifndef GUID_DEVCLASS_PORTS
DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
#endif

static void full_name_win(const std::string &name, std::string &result, boost::system::error_code &ec)
{
    result.clear();
    if (name[0] != '\\')
        result.append("\\\\.\\");

    result.append(name);

    if (result[4] != 'C' || result[5] != 'O' || result[6] != 'M')
    {
        ec = boost::system::error_code(boost::system::errc::bad_file_descriptor,
                                       boost::system::get_system_category());
    }
    else
    {
        ec = boost::system::error_code();
    }
}

static std::string get_reg_key_value(HKEY key, const std::string &property)
{
    DWORD size = 0;
    DWORD type;
    boost::scoped_array<BYTE> buff;
    RegQueryValueExA(key, property.c_str(), NULL, NULL, NULL, &size);
    buff.reset(new BYTE[size]);
    std::string ret;

    if (RegQueryValueExA(key, property.c_str(), NULL, &type, buff.get(), &size) == ERROR_SUCCESS)
    {
        ret = std::string((char*) buff.get());
    }
    return ret;
}

static StringVector enumerate_device_win(const GUID *guid)
{
    StringVector arr;
    HDEVINFO dev_info;
    if ((dev_info = SetupDiGetClassDevsA(guid, NULL, NULL, DIGCF_PRESENT)) != INVALID_HANDLE_VALUE)
    {
        SP_DEVINFO_DATA dev_info_data;
        DWORD i;
        dev_info_data.cbSize = sizeof (SP_DEVINFO_DATA);
        for (i = 0; SetupDiEnumDeviceInfo(dev_info, i, &dev_info_data); ++i)
        {
            HKEY dev_key = SetupDiOpenDevRegKey(dev_info, &dev_info_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            std::string name = get_reg_key_value(dev_key, "PortName");
            std::string fullName;
            boost::system::error_code ec;
            full_name_win(name, fullName, ec);
            if (ec)
            {
                continue;
            }
            arr.push_back(fullName);
        }
    }
    return arr;
}

StringVector serial_port::get_system_ports()
{
    return enumerate_device_win(&GUID_DEVCLASS_PORTS);
}

boost::system::error_code serial_port::set_dtr(bool set, boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return ec;
    }

    if (set)
    {
        if (!EscapeCommFunction(native_handle(), SETDTR))
        {
            ec = boost::system::error_code(GetLastError(), boost::system::get_generic_category());
            return ec;
        }
    }
    else
    {
        if (!EscapeCommFunction(native_handle(), CLRDTR))
        {
            ec = boost::system::error_code(GetLastError(), boost::system::get_generic_category());
            return ec;
        }
    }
    ec = boost::system::error_code();
    return ec;
}

boost::system::error_code serial_port::set_rts(bool set, boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = ec = boost::asio::error::not_connected;
        return ec;
    }

    DWORD dw_func;

    if (set)
    {
        dw_func = SETRTS;
    }
    else
    {
        dw_func = CLRRTS;
    }

    if (!EscapeCommFunction(native_handle(), dw_func))
    {
        ec = boost::system::error_code(GetLastError(), boost::system::get_system_category());
        return ec;
    }
    ec = boost::system::error_code();
    return ec;
}

std::size_t serial_port::bytes_available(boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return 0;
    }

    DWORD errors;
    COMSTAT status;

    if (!ClearCommError(native_handle(), &errors, &status))
    {
        ec = boost::system::error_code(GetLastError(), boost::system::get_system_category());
        return 0;
    }

    return static_cast<std::size_t> (status.cbInQue);
}

boost::system::error_code serial_port::discard_in_buffer(boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return ec;
    }

    if (!PurgeComm(native_handle(), PURGE_RXABORT | PURGE_RXCLEAR))
    {
        ec = boost::system::error_code(GetLastError(), boost::system::get_system_category());
        return ec;
    }

    ec = boost::system::error_code();
    return ec;
}

boost::system::error_code serial_port::discard_out_buffer(boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return ec;
    }
    
    if (!PurgeComm(native_handle(), PURGE_TXABORT | PURGE_TXCLEAR))
    {
        ec = boost::system::error_code(GetLastError(), boost::system::get_system_category());
        return ec;
    }
    
    ec = boost::system::error_code();
    return ec;
}

#else

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

boost::system::error_code serial_port::set_dtr(bool set, boost::system::error_code &ec)
{
    if (boost::asio::serial_port::native_handle() == -1)
    {
        ec = boost::asio::error::not_connected;
        return ec;
    }

    int status;
    int ret = ioctl(boost::asio::serial_port::native_handle(), TIOCMGET, &status);
    if (ret != 0)
    {
        ec = boost::system::error_code(ret, boost::system::get_generic_category());
        return;
    }

    if (set)
    {
        status |= TIOCM_DTR;
    }
    else
    {
        status &= ~TIOCM_DTR;
    }

    ret = ioctl(boost::asio::serial_port::native_handle(), TIOCMSET, &status);
    if (ret != 0)
    {
        ec = boost::system::error_code(ret, boost::system::get_generic_category());
        return;
    }
    ec = boost::system::error_code();
}

boost::system::error_code serial_port::set_rts(bool set, boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return ec;
    }

    int status;
    int ret = ioctl(native_handle(), TIOCMGET, &status);
    if (ret != 0)
    {
        ec = boost::system::error_code(ret, boost::system::get_system_category());
        return ec;
    }

    if (set)
    {
        status |= TIOCM_RTS;
    }
    else
    {
        status &= ~TIOCM_RTS;
    }

    ret = ioctl(native_handle(), TIOCMSET, &status);

    ec = boost::system::error_code(ret, boost::system::get_system_category());
    return ec;
}

StringVector serial_port::get_system_ports()
{
    StringVector arr;
    struct dirent *ent = NULL;
    DIR *dir_p;
    char dir[512];
    struct stat statbuf;

    if ((dir_p = opendir("/dev")) == NULL)
    {
        return arr;
    }

    while ((ent = readdir(dir_p)) != NULL)
    {
        /* get file absolute name */
        snprintf(dir, 512, "%s/%s", "/dev", ent->d_name);
        /* get file information */
        lstat(dir, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            /* skip directory */
        }
        else
        {
            if ((ent->d_name[0] == 't') && (ent->d_name[1] == 't')
                && (ent->d_name[2] == 'y'))
            {
                if ((ent->d_name[3] == 'S') ||
                    ((ent->d_name[3] == 'U') &&
                    (ent->d_name[4] == 'S') &&
                    (ent->d_name[5] == 'B')) ||
                    ((ent->d_name[3] == 'A') &&
                    (ent->d_name[4] == 'C') &&
                    (ent->d_name[5] == 'M')))
                {
                    arr.push_back(ent->d_name);
                }
            }
            else if ((ent->d_name[0] == 'r') && (ent->d_name[1] == 'f')
                && (ent->d_name[2] == 'c') && (ent->d_name[3] == 'o')
                && (ent->d_name[4] == 'm') && (ent->d_name[5] == 'm'))
            {
                arr.push_back(ent->d_name);
            }
        }
    }
    return arr;
}

std::size_t serial_port::bytes_available(boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return 0;
    }

    int bytes_queued;
    int ret = ioctl(native_handle(), FIONREAD, &bytes_queued);

    if (ret)
    {
        ec = boost::system::error_code(ret, boost::system::get_system_category());
        return 0;
    }

    return static_cast<std::size_t> (bytes_queued);
}

boost::system::error_code serial_port::discard_in_buffer(boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return ec;
    }

    int ret = tcflush(native_handle(), TCIFLUSH);
    if (ret)
    {
        ec = boost::system::error_code(ret, boost::system::get_system_category());
        return ec;
    }

    ec = boost::system::error_code();
    return ec;
}

boost::system::error_code serial_port::discard_out_buffer(boost::system::error_code &ec)
{
    if (!is_open())
    {
        ec = boost::asio::error::not_connected;
        return ec;
    }
    
    int ret = tcflush(native_handle(), TCOFLUSH);
    if (ret)
    {
        ec = boost::system::error_code(ret, boost::system::get_system_category());
        return ec;
    }
    
    ec = boost::system::error_code();
    return ec;
}

#endif

}
}
