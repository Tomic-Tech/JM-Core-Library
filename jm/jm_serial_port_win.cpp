#include "jm_serial_port.hpp"
#include "jm_serial_port_private.hpp"
#include "jm_log.hpp"

#ifdef JM_OS_WIN

// see http://msdn.microsoft.com/en-us/library/ms791134.aspx for list of GUID classes
#ifndef GUID_DEVCLASS_PORTS
DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08,
        0x00, 0x2B, 0xE1, 0x03, 0x18);
#endif

namespace jm {

void serial_port::platform_initialize() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    _pri->_bytes_to_write = 0;
    _pri->_handle = INVALID_HANDLE_VALUE;
    ZeroMemory(&_pri->_comm_config, sizeof (COMMCONFIG));
    ZeroMemory(&_pri->_comm_timeouts, sizeof (COMMTIMEOUTS));
    ZeroMemory(&_pri->_read_overlap, sizeof (OVERLAPPED));
    ZeroMemory(&_pri->_write_overlap, sizeof (OVERLAPPED));
    _pri->_read_overlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    _pri->_write_overlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    set_port_name("\\\\.\\COM1");
}

void serial_port::platform_destroy() {

}

error_code serial_port::set_port_name(const std::string &name) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (name.empty()) {
        return error::generic_error;
    }
    if (is_open()) {
        return error::generic_error;
    }
    return _pri->full_name_win(name, _pri->_port_name);
}

error_code serial_port::set_baudrate(int32 baudrate) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (_pri->_baudrate != baudrate) {
        switch (baudrate) {
        case 50:
        case 75:
        case 134:
        case 150:
        case 200:
        case 1800:
        case 76800:
            return error::generic_error;
        default:
            _pri->_baudrate = baudrate;
            break;
        }
    }
    if (!is_open()) {
        return error::success;
    }
    switch (baudrate) {
    case 110:
        _pri->_comm_config.dcb.BaudRate = CBR_110;
        break;
    case 300:
        _pri->_comm_config.dcb.BaudRate = CBR_300;
        break;
    case 600:
        _pri->_comm_config.dcb.BaudRate = CBR_600;
        break;
    case 1200:
        _pri->_comm_config.dcb.BaudRate = CBR_1200;
        break;
    case 2400:
        _pri->_comm_config.dcb.BaudRate = CBR_2400;
        break;
    case 4800:
        _pri->_comm_config.dcb.BaudRate = CBR_4800;
        break;
    case 9600:
        _pri->_comm_config.dcb.BaudRate = CBR_9600;
        break;
    case 14400:
        _pri->_comm_config.dcb.BaudRate = CBR_14400;
        break;
    case 19200:
        _pri->_comm_config.dcb.BaudRate = CBR_19200;
        break;
    case 38400:
        _pri->_comm_config.dcb.BaudRate = CBR_38400;
        break;
    case 57600:
        log_inst().write(log::debug, "serial port", "set baudrate 57600");
        _pri->_comm_config.dcb.BaudRate = CBR_57600;
        break;
    case 115200:
        _pri->_comm_config.dcb.BaudRate = CBR_115200;
        break;
    case 128000:
        _pri->_comm_config.dcb.BaudRate = CBR_128000;
        break;
    case 256000:
        _pri->_comm_config.dcb.BaudRate = CBR_256000;
        break;
    default:
        return error::generic_error;
    }

    if (_pri->_is_multi_setting)
        return error::success;

    if (!SetCommConfig(_pri->_handle, &(_pri->_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return error::success;
}

error_code serial_port::set_databits(uint8 databits) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (databits < 5 || databits > 8) {
        return error::generic_error;
    }
    if (_pri->_databits != databits) {
        if (_pri->_stopbits == stopbits_two && databits == 5) {
            return error::generic_error;
        } else if (_pri->_stopbits == stopbits_one_point_five && databits != 5) {
            return error::generic_error;
        } else if (_pri->_parity == parity_space && databits == 8) {
            return error::generic_error;
        } else {
            _pri->_databits = databits;
        }
    }

    if (!is_open()) {
        return error::success;
    }

    switch (databits) {
    case 5:
        if (_pri->_stopbits == stopbits_two) {
            return error::generic_error;
        } else {
            _pri->_comm_config.dcb.ByteSize = 5;
        }
        break;
    case 6:
        if (_pri->_stopbits == stopbits_one_point_five) {
            return error::generic_error;
        } else {
            _pri->_comm_config.dcb.ByteSize = 6;
        }
        break;
    case 7:
        if (_pri->_stopbits == stopbits_one_point_five) {
            return error::generic_error;
        } else {
            _pri->_comm_config.dcb.ByteSize = 7;
        }
        break;
    case 8:
        if (_pri->_stopbits == stopbits_one_point_five) {
            return error::generic_error;
        } else {
            _pri->_comm_config.dcb.ByteSize = 8;
        }
        break;
    }

    if (_pri->_is_multi_setting)
        return error::success;

    if (!SetCommConfig(_pri->_handle, &(_pri->_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return error::success;
}

error_code serial_port::set_parity(int32 parity) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!(parity == parity_even || parity == parity_mark || parity == parity_none || parity == parity_odd || parity == parity_space)) {
        return error::generic_error;
    }

    if (_pri->_parity != parity) {
        _pri->_parity = parity;
    }

    if (!is_open()) {
        return error::success;
    }

    _pri->_comm_config.dcb.Parity = (BYTE) parity;
    switch (parity) {
    case parity_space:
        if (_pri->_databits == 8) {
            log_inst().write(log::warn, "SerialPort", "Space parity with 8 data bits is not supported by POSIX systems.");
        }
        _pri->_comm_config.dcb.fParity = TRUE;
        break;
    case parity_mark:
        log_inst().write(log::warn, "SerialPort", "Mark parity is not supported by POSIX systems.");
        _pri->_comm_config.dcb.fParity = TRUE;
        break;
    case parity_none:
        _pri->_comm_config.dcb.fParity = FALSE;
        break;
    case parity_even:
        _pri->_comm_config.dcb.fParity = TRUE;
        break;
    case parity_odd:
        _pri->_comm_config.dcb.fParity = TRUE;
        break;
    }

    if (_pri->_is_multi_setting)
        return error::success;

    if (!SetCommConfig(_pri->_handle, &(_pri->_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return error::success;
}

error_code serial_port::set_stopbits(int32 stopbits) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!(stopbits == stopbits_one || stopbits == stopbits_one_point_five || stopbits == stopbits_two)) {
        return error::generic_error;
    }
    if (_pri->_stopbits != stopbits) {
        if (_pri->_databits == 5 && stopbits == stopbits_two) {
            return error::generic_error;
        } else if (stopbits == stopbits_one_point_five && _pri->_databits != 5) {
            return error::generic_error;
        } else {
            _pri->_stopbits = stopbits;
        }
    }

    if (!is_open()) {
        return error::success;
    }

    switch (stopbits) {
    case stopbits_one:
        _pri->_comm_config.dcb.StopBits = ONESTOPBIT;
        break;
    case stopbits_one_point_five:
        log_inst().write(log::warn, "SerialPort", "1.5 stop bit operation is not supported by POSIX.");
        if (_pri->_databits != 5) {
            return -1;
        } else {
            _pri->_comm_config.dcb.StopBits = ONE5STOPBITS;
        }
        break;
    case stopbits_two:
        if (_pri->_databits == 5) {
            return error::generic_error;
        } else {
            _pri->_comm_config.dcb.StopBits = TWOSTOPBITS;
        }
        break;
    }

    if (_pri->_is_multi_setting)
        return error::success;

    if (!SetCommConfig(_pri->_handle, &(_pri->_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return error::success;
}

error_code serial_port::set_flow_control(int32 flow_contorl) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!(_pri->_flow_control == flow_control_hardware || _pri->_flow_control == flow_control_software || _pri->_flow_control == flow_control_none)) {
        return error::generic_error;
    }

    if (_pri->_flow_control != _pri->_flow_control) {
        _pri->_flow_control = _pri->_flow_control;
    }

    if (!is_open()) {
        return error::success;
    }

    switch (_pri->_flow_control) {
    case flow_control_none:
        _pri->_comm_config.dcb.fOutxCtsFlow = FALSE;
        _pri->_comm_config.dcb.fRtsControl = RTS_CONTROL_DISABLE;
        _pri->_comm_config.dcb.fInX = FALSE;
        _pri->_comm_config.dcb.fOutX = FALSE;
        break;

    case flow_control_software:
        _pri->_comm_config.dcb.fOutxCtsFlow = FALSE;
        _pri->_comm_config.dcb.fRtsControl = RTS_CONTROL_DISABLE;
        _pri->_comm_config.dcb.fInX = TRUE;
        _pri->_comm_config.dcb.fOutX = TRUE;
        break;

    case flow_control_hardware:
        _pri->_comm_config.dcb.fOutxCtsFlow = TRUE;
        _pri->_comm_config.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        _pri->_comm_config.dcb.fInX = FALSE;
        _pri->_comm_config.dcb.fOutX = FALSE;
        break;
    default:
        break;
    }

    if (_pri->_is_multi_setting)
        return error::success;

    if (!SetCommConfig(_pri->_handle, &(_pri->_comm_config), sizeof (COMMCONFIG))) {
        return GetLastError();
    }
    return error::success;
}

error_code serial_port::set_write_timeout(int64 millic_seconds) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    _pri->_write_timeout = millic_seconds;

    if (!is_open()) {
        return error::success;
    }

    if (millic_seconds == -1) {
        _pri->_comm_timeouts.WriteTotalTimeoutConstant = MAXDWORD;
        _pri->_comm_timeouts.WriteTotalTimeoutMultiplier = MAXDWORD;
    } else {
        _pri->_comm_timeouts.WriteTotalTimeoutConstant = static_cast<DWORD> (millic_seconds);
        _pri->_comm_timeouts.WriteTotalTimeoutMultiplier = 0;
    }

    if (_pri->_is_multi_setting)
        return error::success;

    if (!SetCommTimeouts(_pri->_handle, &(_pri->_comm_timeouts))) {
        return GetLastError();
    }

    return error::success;
}

error_code serial_port::set_read_timeout(int64 millic_seconds) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    _pri->_read_timeout = millic_seconds;

    if (!is_open()) {
        return error::success;
    }

    if (millic_seconds == -1) {
        _pri->_comm_timeouts.ReadIntervalTimeout = MAXDWORD;
        _pri->_comm_timeouts.ReadTotalTimeoutConstant = 0;
    } else {
        _pri->_comm_timeouts.ReadIntervalTimeout = static_cast<DWORD> (millic_seconds);
        _pri->_comm_timeouts.ReadTotalTimeoutConstant = static_cast<DWORD> (millic_seconds);
    }
    _pri->_comm_timeouts.ReadTotalTimeoutMultiplier = 0;
    if (_pri->_is_multi_setting)
        return error::success;
    if (!SetCommTimeouts(_pri->_handle, &(_pri->_comm_timeouts))) {
        return GetLastError();
    }

    return error::success;
}

bool serial_port::is_open() {
    return _pri->_handle != INVALID_HANDLE_VALUE;
}

error_code serial_port::open() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    ulong conf_size = sizeof (COMMCONFIG);
    DWORD dw_flags_and_attribute = FILE_FLAG_OVERLAPPED;


    _pri->_comm_config.dwSize = conf_size;

    if (is_open()) {
        return error::generic_error;
    }

    /* Open the port */
    _pri->_handle = CreateFileA(_pri->_port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, dw_flags_and_attribute, NULL);

    if (_pri->_handle == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    /* Configure port setting */
    if (!GetCommConfig(_pri->_handle, &(_pri->_comm_config), &conf_size) || !GetCommState(_pri->_handle, &(_pri->_comm_config.dcb))) {
        return GetLastError();
    }

    /* Set up parameters */
    _pri->_comm_config.dcb.fBinary = TRUE;
    _pri->_comm_config.dcb.fInX = FALSE;
    _pri->_comm_config.dcb.fOutX = FALSE;
    _pri->_comm_config.dcb.fAbortOnError = FALSE;
    _pri->_comm_config.dcb.fNull = FALSE;
    _pri->_is_multi_setting = true;
    if (set_baudrate(_pri->_baudrate)
            || set_databits(_pri->_databits)
            || set_stopbits(_pri->_stopbits)
            || set_parity(_pri->_parity)
            || set_flow_control(_pri->_flow_control)
            || set_read_timeout(_pri->_read_timeout)
            || set_write_timeout(_pri->_write_timeout)) {
        _pri->_is_multi_setting = false;
        return error::generic_error;
    }
    if (!SetCommConfig(_pri->_handle, &(_pri->_comm_config), sizeof (COMMCONFIG))) {
        _pri->_is_multi_setting = false;
        return GetLastError();
    }
    if (!SetCommTimeouts(_pri->_handle, &(_pri->_comm_timeouts))) {
        _pri->_is_multi_setting = false;
        return GetLastError();
    }

    _pri->_is_multi_setting = false;

    //    _pri->_comm_timeouts.ReadIntervalTimeout = MAXDWORD;
    //    _pri->_comm_timeouts.ReadTotalTimeoutMultiplier = 0;
    //    _pri->_comm_timeouts.ReadTotalTimeoutConstant = 0;
    //    _pri->_comm_timeouts.WriteTotalTimeoutMultiplier = 0;
    //    _pri->_comm_timeouts.WriteTotalTimeoutConstant = 0;
    //    if (!SetCommTimeouts(_pri->_handle, &_pri->_comm_timeouts))
    //        throw IOException() << ErrorString("JM::IO::SerialPort: Failed to set Comm Mask") << error_code(::GetLastError());
    //    if (!SetCommMask(_pri->_handle, EV_TXEMPTY | EV_RXCHAR | EV_DSR))
    //        throw IOException() << ErrorString("JM::IO::SerialPort: Failed to set Comm Mask") << error_code(::GetLastError());

    return error::success;
}

error_code serial_port::close() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!is_open()) {
        return error::success;
    }

    _pri->_bytes_to_write = 0;

    // Force a flush
    flush();
    CancelIo(_pri->_handle);
    if (CloseHandle(_pri->_handle)) {
        _pri->_handle = INVALID_HANDLE_VALUE;
    } else {
        return error::generic_error;
    }
    return error::success;
}

error_code serial_port::flush() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!is_open()) {
        return error::generic_error;
    }

    if (!FlushFileBuffers(_pri->_handle)) {
        return error::generic_error;
    }
    return error::success;
}

error_code serial_port::discard_in_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!is_open()) {
        return error::generic_error;
    }

    if (!PurgeComm(_pri->_handle, PURGE_RXABORT | PURGE_RXCLEAR)) {
        return error::generic_error;
    }
    return error::success;
}

error_code serial_port::discard_out_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!is_open()) {
        return error::generic_error;
    }

    if (!PurgeComm(_pri->_handle, PURGE_TXABORT | PURGE_TXCLEAR)) {
        return error::generic_error;
    }
    return error::success;
}

size_type serial_port::bytes_available() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    DWORD errors;
    COMSTAT status;

    if (!is_open()) {
        return 0;
    }

    if (!ClearCommError(_pri->_handle, &errors, &status)) {
        return 0;
    }

    return static_cast<size_type> (status.cbInQue);
}

error_code serial_port::set_dtr(bool set) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!is_open()) {
        return error::generic_error;
    }

    DWORD dwFunc;
    if (set)
        dwFunc = SETDTR;
    else
        dwFunc = CLRDTR;

    if (!EscapeCommFunction(_pri->_handle, dwFunc)) {
        return error::generic_error;
    }
    return error::success;
}

error_code serial_port::set_rts(bool set) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (!is_open()) {
        return error::generic_error;
    }

    DWORD dwFunc;
    if (set)
        dwFunc = SETRTS;
    else
        dwFunc = CLRRTS;

    if (!EscapeCommFunction(_pri->_handle, dwFunc)) {
        return error::generic_error;
    }

    return error::success;
}

size_type serial_port::read(uint8 *data, size_type offset, size_type count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (data == NULL) {
        return 0;
    }

    if (count <= 0) {
        return 0;
    }

    if (!is_open()) {
        return 0;
    }

    DWORD ret_val = -1;
    if (!ReadFile(_pri->_handle, (void*) (data + offset), (DWORD) count, &ret_val, &_pri->_read_overlap)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return 0;
        }
        GetOverlappedResult(_pri->_handle, &_pri->_read_overlap, &ret_val, TRUE);
    }
    log_inst().write_hex(log::debug, "serial port: read ", data, offset, ret_val);
    return static_cast<size_type> (ret_val);
}

size_type serial_port::write(const uint8 *data, size_type offset, size_type count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (data == NULL) {
        return 0;
    }

    if (count <= 0) {
        return 0;
    }

    if (!is_open()) {
        return 0;
    }

    log_inst().write_hex(log::debug, "serial port: write ", data, offset, count);
    DWORD ret_val = -1;
    if (!WriteFile(_pri->_handle, (const void*) (data + offset), (DWORD) count, &ret_val, &_pri->_write_overlap)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return 0;
        }
        WaitForSingleObject(_pri->_write_overlap.hEvent, INFINITE);
        GetOverlappedResult(_pri->_handle, &_pri->_write_overlap, &ret_val, TRUE);
    }
    return static_cast<size_type> (ret_val);
}

std::vector<std::string> serial_port::get_system_ports() {
    return serial_port_private::enumerate_device_win(&GUID_DEVCLASS_PORTS);
}

}

#endif