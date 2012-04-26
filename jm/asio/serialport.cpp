#include "serialport.hpp"
#include <boost/system/error_code.hpp>
#include <boost/array.hpp>
#include <boost/smart_ptr.hpp>

#ifdef BOOST_WINDOWS
#include <Windows.h>
#include <ObjBase.h>
#include <InitGuid.h>
#include <SetupAPI.h>
#include <Dbt.h>
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
#endif

namespace JM
{
namespace Asio
{
#ifdef BOOST_WINDOWS
// see http://msdn.microsoft.com/en-us/library/ms791134.aspx for list of GUID classes
#ifndef GUID_DEVCLASS_PORTS
DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
#endif

static void FullNameWin(const std::string &name, std::string &result,
                        boost::system::error_code &ec)
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

static std::string GetRegKeyValue(HKEY key, const std::string &property)
{
    DWORD size = 0;
    DWORD type;
    boost::scoped_array<BYTE> buff;
    RegQueryValueExA(key, property.c_str(), NULL, NULL, NULL, &size);
    buff.reset(new BYTE[size]);
    std::string ret;

    if (RegQueryValueExA(key, property.c_str(), NULL, &type, buff.get(), &size) == ERROR_SUCCESS)
    {
        ret = std::string((char*)buff.get());
    }
    return ret;
}

static StringVector EnumerateDeviceWin(const GUID* guid)
{
    StringVector arr;
    HDEVINFO dev_info;
    if ((dev_info = SetupDiGetClassDevsA(guid, NULL, NULL, DIGCF_PRESENT)) != INVALID_HANDLE_VALUE)
    {
        SP_DEVINFO_DATA dev_info_data;
        DWORD i;
        dev_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
        for (i = 0; SetupDiEnumDeviceInfo(dev_info, i, &dev_info_data); ++i)
        {
            HKEY dev_key = SetupDiOpenDevRegKey(dev_info, &dev_info_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            std::string name = GetRegKeyValue(dev_key, "PortName");
            std::string fullName;
            boost::system::error_code ec;
            FullNameWin(name, fullName, ec);
            if (ec)
            {
                continue;
            }
            arr.push_back(fullName);
        }
    }
    return arr;
}

StringVector SerialPort::getSystemPorts()
{
    return EnumerateDeviceWin(&GUID_DEVCLASS_PORTS);
}

void SerialPort::setDtr(HANDLE handle, bool set,
                        boost::system::error_code &ec)
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        ec = boost::asio::error::not_connected;
        return;
    }

    if (set)
    {
        if (!EscapeCommFunction(handle, SETDTR))
        {
            ec = boost::system::error_code(GetLastError(),
                                           boost::system::get_generic_category());
            return;
        }
    }
    else
    {
        if (!EscapeCommFunction(handle, CLRDTR))
        {
            ec = boost::system::error_code(GetLastError(),
                                           boost::system::get_generic_category());
            return;
        }
    }
    ec = boost::system::error_code();
}

std::size_t SerialPort::bytesAvailable(HANDLE handle,
                                       boost::system::error_code &ec)
{
    DWORD errors;
    COMSTAT status;
    if (handle == INVALID_HANDLE_VALUE)
    {
        ec = boost::asio::error::not_connected;
        return 0;
    }

    if (!ClearCommError(handle, &errors, &status))
    {
        ec = boost::system::error_code(GetLastError(), boost::system::get_generic_category());
        return 0;
    }
    ec = boost::system::error_code();
    return (std::size_t)status.cbInQue;
}
#else
void SerialPort::setDtr(int fd, bool set, boost::system::error_code &ec)
{
    if (fd == -1)
    {
        ec = boost::asio::error::not_connected;
        return;
    }

    int status;
    int ret = ioctl(fd, TIOCMGET, &status);
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

    ret = ioctl(fd, TIOCMSET, &status);
    if (ret != 0)
    {
        ec = boost::system::error_code(ret, boost::system::get_generic_category());
        return;
    }
    ec = boost::system::error_code();
}

std::size_t SerialPort::bytesAvailable(int fd, boost::system::error_code &ec)
{
    if (fd == -1)
    {
        ec = boost::asio::error::not_connected;
        return 0;
    }

    int bytesQueued;

    ret = ioctl(fd, FIONREAD, &bytesQueued);
    if (ret != 0)
    {
        ec = boost::system::error_code(ret, boost::system::get_generic_category());
        return 0;
    }

    ec = boost::system::error_code();
    return static_cast<std::size_t>(bytesQueued);
}

StringVector SerialPort::getSystemPorts()
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

    while ((ent == readdir(dir_p)) != NULL)
    {
        // get file absolute name
        snprintf(dir, 512, "%s/%s", "/dev", ent->d_name);
        // get file information
        lstat(dir, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            // skip directory
        }
        else
        {
            if ((ent->d_name[0] == 't') && (ent->d_name[1] == 't') &&
                (ent->d_name[2] == 'y'))
            {
                if ((ent->d_name[3] == 'S') || 
                    ((ent->d_name[3] == 'U') && (ent->d_name[4] == 'S') && (ent->d_name[5] == 'B')) ||
                    ((ent->d_name[3] == 'A') && (ent->d_name[4] == 'C') && (ent->d_name[5] == 'M')))
                {
                    arr.push_back(ent->d_name);
                }
            }
            else if ((ent->d_name[0] == 'r') && (ent->d_name[1] == 'f') && 
                (ent->d_name[2] == 'c') && (ent->d_name[3] == 'o') &&
                (ent->d_name[4] == 'm') && (ent->d_name[5] == 'm'))
            {
                arr.push_back(ent->d_name);
            }
        }
    }
    return arr;
}
#endif
}
}