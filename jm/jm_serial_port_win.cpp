#include "jm_serial_port.hpp"
#include "jm_log.hpp"

#ifdef JM_OS_WIN

// see http://msdn.microsoft.com/en-us/library/ms791134.aspx for list of GUID classes
#ifndef GUID_DEVCLASS_PORTS
DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08,
        0x00, 0x2B, 0xE1, 0x03, 0x18);
#endif

namespace jm {

void serial_port::platform_initialize() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    _bytes_to_write = 0;
    _handle = INVALID_HANDLE_VALUE;
    ZeroMemory(&_comm_config, sizeof (COMMCONFIG));
    ZeroMemory(&_comm_timeouts, sizeof (COMMTIMEOUTS));
    ZeroMemory(&_read_overlap, sizeof (OVERLAPPED));
    ZeroMemory(&_write_overlap, sizeof (OVERLAPPED));
    _read_overlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    _write_overlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    set_port_name("\\\\.\\COM1");
}

void serial_port::platform_destroy() {

}

int32 serial_port::set_port_name(const std::string &name) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (name.empty()) {
        return -1;
    }
    if (is_open()) {
        return -1;
    }
    return full_name_win(name, _port_name);
}

int32 serial_port::full_name_win(const std::string &name, std::string &result) {
    result.clear();
    if (name[0] != '\\')
        result += "\\\\.\\";

    result += name;
    if (result[4] != 'C' || result[5] != 'O' || result[6] != 'M') {
        return -1;
    }
    return 0;
}

int32 serial_port::set_baudrate(int32 baudrate) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (_baudrate != baudrate) {
        switch (baudrate) {
        case 50:
        case 75:
        case 134:
        case 150:
        case 200:
        case 1800:
        case 76800:
            return -1;
        default:
            _baudrate = baudrate;
            break;
        }
    }
    if (!is_open()) {
        return 0;
    }
    switch (baudrate) {
    case 110:
        _comm_config.dcb.BaudRate = CBR_110;
        break;
    case 300:
        _comm_config.dcb.BaudRate = CBR_300;
        break;
    case 600:
        _comm_config.dcb.BaudRate = CBR_600;
        break;
    case 1200:
        _comm_config.dcb.BaudRate = CBR_1200;
        break;
    case 2400:
        _comm_config.dcb.BaudRate = CBR_2400;
        break;
    case 4800:
        _comm_config.dcb.BaudRate = CBR_4800;
        break;
    case 9600:
        _comm_config.dcb.BaudRate = CBR_9600;
        break;
    case 14400:
        _comm_config.dcb.BaudRate = CBR_14400;
        break;
    case 19200:
        _comm_config.dcb.BaudRate = CBR_19200;
        break;
    case 38400:
        _comm_config.dcb.BaudRate = CBR_38400;
        break;
    case 57600:
        log_inst().write(log::debug, "serial port", "set baudrate 57600");
        _comm_config.dcb.BaudRate = CBR_57600;
        break;
    case 115200:
        _comm_config.dcb.BaudRate = CBR_115200;
        break;
    case 128000:
        _comm_config.dcb.BaudRate = CBR_128000;
        break;
    case 256000:
        _comm_config.dcb.BaudRate = CBR_256000;
        break;
    default:
        return -1;
    }

    if (_is_multi_setting)
        return 0;

    if (!SetCommConfig(_handle, &(_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return 0;
}

int32 serial_port::set_databits(uint8 databits) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (databits < 5 || databits > 8) {
        return -1;
    }
    if (_databits != databits) {
        if (_stopbits == stopbits_two && databits == 5) {
            return -1;
        } else if (_stopbits == stopbits_one_point_five && databits != 5) {
            return -1;
        } else if (_parity == parity_space && databits == 8) {
            return -1;
        } else {
            _databits = databits;
        }
    }

    if (!is_open()) {
        return 0;
    }

    switch (databits) {
    case 5:
        if (_stopbits == stopbits_two) {
            return -1;
        } else {
            _comm_config.dcb.ByteSize = 5;
        }
        break;
    case 6:
        if (_stopbits == stopbits_one_point_five) {
            return -1;
        } else {
            _comm_config.dcb.ByteSize = 6;
        }
        break;
    case 7:
        if (_stopbits == stopbits_one_point_five) {
            return -1;
        } else {
            _comm_config.dcb.ByteSize = 7;
        }
        break;
    case 8:
        if (_stopbits == stopbits_one_point_five) {
            return -1;
        } else {
            _comm_config.dcb.ByteSize = 8;
        }
        break;
    }

    if (_is_multi_setting)
        return 0;

    if (!SetCommConfig(_handle, &(_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return 0;
}

int32 serial_port::set_parity(int32 parity) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!(parity == parity_even || parity == parity_mark || parity == parity_none || parity == parity_odd || parity == parity_space)) {
        return -1;
    }

    if (_parity != parity) {
        _parity = parity;
    }

    if (!is_open()) {
        return 0;
    }

    _comm_config.dcb.Parity = (BYTE) parity;
    switch (parity) {
    case parity_space:
        if (_databits == 8) {
            log_inst().write(log::warn, "SerialPort", "Space parity with 8 data bits is not supported by POSIX systems.");
        }
        _comm_config.dcb.fParity = TRUE;
        break;
    case parity_mark:
        log_inst().write(log::warn, "SerialPort", "Mark parity is not supported by POSIX systems.");
        _comm_config.dcb.fParity = TRUE;
        break;
    case parity_none:
        _comm_config.dcb.fParity = FALSE;
        break;
    case parity_even:
        _comm_config.dcb.fParity = TRUE;
        break;
    case parity_odd:
        _comm_config.dcb.fParity = TRUE;
        break;
    }

    if (_is_multi_setting)
        return 0;

    if (!SetCommConfig(_handle, &(_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return 0;
}

int32 serial_port::set_stopbits(int32 stopbits) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!(stopbits == stopbits_one || stopbits == stopbits_one_point_five || stopbits == stopbits_two)) {
        return -1;
    }
    if (_stopbits != stopbits) {
        if (_databits == 5 && stopbits == stopbits_two) {
            return -1;
        } else if (stopbits == stopbits_one_point_five && _databits != 5) {
            return -1;
        } else {
            _stopbits = stopbits;
        }
    }

    if (!is_open()) {
        return 0;
    }

    switch (stopbits) {
    case stopbits_one:
        _comm_config.dcb.StopBits = ONESTOPBIT;
        break;
    case stopbits_one_point_five:
        log_inst().write(log::warn, "SerialPort", "1.5 stop bit operation is not supported by POSIX.");
        if (_databits != 5) {
            return -1;
        } else {
            _comm_config.dcb.StopBits = ONE5STOPBITS;
        }
        break;
    case stopbits_two:
        if (_databits == 5) {
            return -1;
        } else {
            _comm_config.dcb.StopBits = TWOSTOPBITS;
        }
        break;
    }

    if (_is_multi_setting)
        return 0;

    if (!SetCommConfig(_handle, &(_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return 0;
}

int32 serial_port::set_flow_control(int32 flow_contorl) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!(_flow_control == flow_control_hardware || _flow_control == flow_control_software || _flow_control == flow_control_none)) {
        return -1;
    }

    if (_flow_control != _flow_control) {
        _flow_control = _flow_control;
    }

    if (!is_open()) {
        return 0;
    }

    switch (_flow_control) {
    case flow_control_none:
        _comm_config.dcb.fOutxCtsFlow = FALSE;
        _comm_config.dcb.fRtsControl = RTS_CONTROL_DISABLE;
        _comm_config.dcb.fInX = FALSE;
        _comm_config.dcb.fOutX = FALSE;
        break;

    case flow_control_software:
        _comm_config.dcb.fOutxCtsFlow = FALSE;
        _comm_config.dcb.fRtsControl = RTS_CONTROL_DISABLE;
        _comm_config.dcb.fInX = TRUE;
        _comm_config.dcb.fOutX = TRUE;
        break;

    case flow_control_hardware:
        _comm_config.dcb.fOutxCtsFlow = TRUE;
        _comm_config.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        _comm_config.dcb.fInX = FALSE;
        _comm_config.dcb.fOutX = FALSE;
        break;
    default:
        break;
    }

    if (!_is_multi_setting)
        return 0;

    if (!SetCommConfig(_handle, &(_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return 0;
}

int32 serial_port::set_write_timeout(int64 millic_seconds) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    _write_timeout = millic_seconds;
    
    if (!is_open()) {
        return 0;
    }
    
    if (millic_seconds == -1) {
        _comm_timeouts.WriteTotalTimeoutConstant = MAXDWORD;
        _comm_timeouts.WriteTotalTimeoutMultiplier = MAXDWORD;
    } else {
        _comm_timeouts.WriteTotalTimeoutConstant = static_cast<DWORD> (millic_seconds);
        _comm_timeouts.WriteTotalTimeoutMultiplier = 0;
    }

    if (_is_multi_setting)
        return 0;
    
    if (!SetCommTimeouts(_handle, &(_comm_timeouts))) {
        return GetLastError();
    }

    return 0;
}

int32 serial_port::set_read_timeout(int64 millic_seconds) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    _read_timeout = millic_seconds;
    
    if (!is_open()) {
        return 0;
    }
    
    if (millic_seconds == -1) {
        _comm_timeouts.ReadIntervalTimeout = MAXDWORD;
        _comm_timeouts.ReadTotalTimeoutConstant = 0;
    } else {
        _comm_timeouts.ReadIntervalTimeout = static_cast<DWORD> (millic_seconds);
        _comm_timeouts.ReadTotalTimeoutConstant = static_cast<DWORD> (millic_seconds);
    }
    _comm_timeouts.ReadTotalTimeoutMultiplier = 0;
    if (_is_multi_setting)
        return 0;
    if (!SetCommTimeouts(_handle, &(_comm_timeouts))) {
        return GetLastError();
    }

    return 0;
}

bool serial_port::is_open() {
    return _handle != INVALID_HANDLE_VALUE;
}

int32 serial_port::open() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    ulong conf_size = sizeof (COMMCONFIG);
    DWORD dw_flags_and_attribute = FILE_FLAG_OVERLAPPED;


    _comm_config.dwSize = conf_size;

    if (is_open()) {
        return -1;
    }

    /* Open the port */
    _handle = CreateFileA(_port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, dw_flags_and_attribute, NULL);

    if (_handle == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    /* Configure port setting */
    if (!GetCommConfig(_handle, &(_comm_config), &conf_size) || !GetCommState(_handle, &(_comm_config.dcb))) {
        return GetLastError();
    }

    /* Set up parameters */
    _comm_config.dcb.fBinary = TRUE;
    _comm_config.dcb.fInX = FALSE;
    _comm_config.dcb.fOutX = FALSE;
    _comm_config.dcb.fAbortOnError = FALSE;
    _comm_config.dcb.fNull = FALSE;
    _is_multi_setting = true;
    if (set_baudrate(_baudrate)
            || set_databits(_databits)
            || set_stopbits(_stopbits)
            || set_parity(_parity)
            || set_flow_control(_flow_control)
            || set_read_timeout(_read_timeout)
            || set_write_timeout(_write_timeout)) {
        _is_multi_setting = false;
        return -1;
    }
    if (!SetCommConfig(_handle, &(_comm_config), sizeof (COMMCONFIG))) {
        _is_multi_setting = false;
        return GetLastError();
    }
    if (!SetCommTimeouts(_handle, &(_comm_timeouts))) {
        _is_multi_setting = false;
        return GetLastError();
    }
    
    _is_multi_setting = false;

    //    _comm_timeouts.ReadIntervalTimeout = MAXDWORD;
    //    _comm_timeouts.ReadTotalTimeoutMultiplier = 0;
    //    _comm_timeouts.ReadTotalTimeoutConstant = 0;
    //    _comm_timeouts.WriteTotalTimeoutMultiplier = 0;
    //    _comm_timeouts.WriteTotalTimeoutConstant = 0;
    //    if (!SetCommTimeouts(_handle, &_comm_timeouts))
    //        throw IOException() << ErrorString("JM::IO::SerialPort: Failed to set Comm Mask") << error_code(::GetLastError());
    //    if (!SetCommMask(_handle, EV_TXEMPTY | EV_RXCHAR | EV_DSR))
    //        throw IOException() << ErrorString("JM::IO::SerialPort: Failed to set Comm Mask") << error_code(::GetLastError());

    return 0;
}

int32 serial_port::close() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return 0;
    }

    _bytes_to_write = 0;

    // Force a flush
    flush();
    CancelIo(_handle);
    if (CloseHandle(_handle)) {
        _handle = INVALID_HANDLE_VALUE;
    } else {
        return -1;
    }
    return 0;
}

int32 serial_port::flush() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    if (!FlushFileBuffers(_handle)) {
        return -1;
    }
    return 0;
}

int32 serial_port::discard_in_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    if (!PurgeComm(_handle, PURGE_RXABORT | PURGE_RXCLEAR)) {
        return -1;
    }
    return 0;
}

int32 serial_port::discard_out_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    if (!PurgeComm(_handle, PURGE_TXABORT | PURGE_TXCLEAR)) {
        return -1;
    }
    return 0;
}

int64 serial_port::bytes_available() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    DWORD errors;
    COMSTAT status;

    if (!is_open()) {
        return -1;
    }

    if (!ClearCommError(_handle, &errors, &status)) {
        return -1;
    }

    return static_cast<int64> (status.cbInQue);
}

int32 serial_port::set_dtr(bool set) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    DWORD dwFunc;
    if (set)
        dwFunc = SETDTR;
    else
        dwFunc = CLRDTR;

    if (!EscapeCommFunction(_handle, dwFunc)) {
        return -1;
    }
    return 0;
}

int32 serial_port::set_rts(bool set) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    DWORD dwFunc;
    if (set)
        dwFunc = SETRTS;
    else
        dwFunc = CLRRTS;

    if (!EscapeCommFunction(_handle, dwFunc)) {
        return -1;
    }

    return 0;
}

int32 serial_port::read(uint8 *data, int32 offset, int32 count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (data == NULL) {
        return -1;
    }
    
    if (count <= 0) {
        return -1;
    }
    
    if (!is_open()) {
        return -1;
    }
    
    DWORD ret_val = -1;
    if (!ReadFile(_handle, (void*)(data + offset), (DWORD)count, &ret_val, &_read_overlap)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return -1;
        }
        GetOverlappedResult(_handle, &_read_overlap, &ret_val, TRUE);
    }
    return static_cast<int32>(ret_val);
}

int32 serial_port::write(const uint8 *data, int32 offset, int32 count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (data == NULL) {
        return -1;
    }
    
    if (count <= 0) {
        return -1;
    }
    
    if (!is_open()) {
        return -1;
    }
    
    DWORD ret_val = -1;
    if (!WriteFile(_handle, (const void*)(data + offset), (DWORD)count, &ret_val, &_write_overlap)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return -1;
        }
        WaitForSingleObject(_write_overlap.hEvent, INFINITE);
        GetOverlappedResult(_handle, &_write_overlap, &ret_val, TRUE);
    }
    return static_cast<int32>(ret_val);
}

std::vector<std::string> serial_port::get_system_ports() {
    return enumerate_device_win(&GUID_DEVCLASS_PORTS);
}

std::vector<std::string> serial_port::enumerate_device_win(const GUID *guid) {
    std::vector<std::string> vec;
    HDEVINFO dev_info;
    if ((dev_info = SetupDiGetClassDevsA(guid, NULL, NULL, DIGCF_PRESENT)) != INVALID_HANDLE_VALUE) {
        SP_DEVINFO_DATA dev_info_data;
        dev_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
        for (int32 i = 0; SetupDiEnumDeviceInfo(dev_info, i, &dev_info_data); ++i) {
            HKEY dev_key = SetupDiOpenDevRegKey(dev_info, &dev_info_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            std::string name = get_reg_key_value(dev_key, "PortName");
            std::string full_name;
            int ret = full_name_win(name, full_name);
            if (ret)
                continue;
            vec.push_back(full_name);
        }
    }
    return vec;
}

std::string serial_port::get_reg_key_value(HKEY key, const std::string &property) {
    DWORD size = 0;
    DWORD type;
    BYTE *buff = NULL;
    RegQueryValueExA(key, property.c_str(), NULL, NULL, NULL, &size);
    buff = new BYTE[size];
    if (RegQueryValueExA(key, property.c_str(), NULL, &type, buff, &size) == ERROR_SUCCESS) {
        std::string ret((char*)buff, size);
        delete[] buff;
        return ret;
    }
    delete[] buff;
    return std::string();
}

}

#endif