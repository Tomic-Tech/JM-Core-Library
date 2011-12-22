#include "jm_serial_port.hpp"
#include "jm_log.hpp"
#ifdef JM_OS_LINUX

namespace jm {

void serial_port::platform_initialize() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    _fd = -1;
    memset(&_comm_config, 0, sizeof (struct termios));
    memset(&_old_termios, 0, sizeof (struct termios));
    memset(&_timeout, 0, sizeof (struct timeval));
    memset(&_copy_timeout, 0, sizeof (struct timeval));
    set_port_name("/dev/ttyS0");
}

void serial_port::platform_destroy() {

}

int32 serial_port::set_baudrate_p(tcflag_t baud) {
#ifdef CBAUD
    _comm_config.c_cflag &= (~CBAUD);
    _comm_config.c_cflag |= baud;
#else
    cfsetispeed(&_comm_config, baud);
    cfsetospeed(&_comm_config, baud);
#endif
    if (_is_multi_setting)
        return 0;
    return tcsetattr(_fd, TCSAFLUSH, &_comm_config);
}

int32 serial_port::set_databits_p(tcflag_t bits) {
    _comm_config.c_cflag &= (~CSIZE);
    _comm_config.c_cflag |= bits;
    if (_is_multi_setting)
        return 0;
    return tcsetattr(_fd, TCSAFLUSH, &_comm_config);
}

int32 serial_port::set_port_name(const std::string &name) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (name.empty()) {
        return -1;
    }

    if (is_open()) {
        return -1;
    }

    _port_name = name;
    return 0;
}

int32 serial_port::set_baudrate(int32 baudrate) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (_baudrate != baudrate) {
        switch (baudrate) {
        case 14400:
        case 56000:
        case 128000:
        case 256000:
            return -1;
        case 76800:
#ifndef B76800
            return -1;
#else
            _baudrate = baudrate;
#endif
            break;
        default:
            _baudrate = baudrate;
            break;
        }
    }

    if (!is_open()) {
        return 0;
    }

    switch (baudrate) {
    case 50:
        log_inst().write(log::warn, "SerialPort", "Windows does no support 50 baud operation.");
        return set_baudrate_p(B50);
    case 75:
        log_inst().write(log::warn, "SerialPort", "Windows does no support 75 baud operation.");
        return set_baudrate_p(B75);
    case 110:
        return set_baudrate_p(B110, ec);
    case 134:
        log_inst().write(log::warn, "SerialPort", "Windows does no support 134.5 baud operation.");
        return set_baudrate_p(B134);
    case 150:
        log_inst().write(log::warn, "SerialPort", "Windows does no support 150 baud operation.");
        return set_baudrate_p(B150);
    case 200:
        log_inst().write(log::warn, "SerialPort", "Windows does no support 200 baud operation.");
        return set_baudrate_p(B200);
    case 300:
        return set_baudrate_p(B300, ec);
    case 600:
        return set_baudrate_p(B600, ec);
    case 1200:
        return set_baudrate_p(B1200, ec);
    case 1800:
        return set_baudrate_p(B1800, ec);
    case 2400:
        return set_baudrate_p(B2400, ec);
    case 4800:
        return set_baudrate_p(B4800, ec);
    case 9600:
        return set_baudrate_p(B9600, ec);
    case 19200:
        return set_baudrate_p(B19200, ec);
    case 38400:
        return set_baudrate_p(B38400, ec);
    case 57600:
        return set_baudrate_p(B57600, ec);
    case 76800:
        log_inst().write(log::warn, "SerialPort", "Windows and some POSIX systems do not support 76800 baud operation.");
#ifdef B76800
        return set_baudrate_p(B76800, ec);
#else
        return -1;
#endif
        break;
    case 115200:
        return set_baudrate_p(B115200, ec);
    case 256000:
    case 128000:
    case 56000:
    case 14400:
    default:
        return -1;
    }
    return -1;
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
            _databits = databits;
            return set_databits_p(CS5, ec);
        }
        break;
    case 6:
        if (_stopbits == stopbits_one_point_five) {
            return -1;
        } else {
            _databits = databits;
            return set_databits_p(CS6, ec);
        }
        break;
    case 7:
        if (_stopbits == stopbits_one_point_five) {
            return -1;
        } else {
            _databits = databits;
            return set_databits_p(CS7, ec);
        }
        break;
    case 8:
        if (_stopbits == stopbits_one_point_five) {
            return -1;
        } else {
            _databits = databits;
            return set_databits_p(CS8, ec);
        }
        break;
    }
    return -1;
}

int32 serial_port::set_parity(int32 parity) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!(parity == parity_even || parity == parity_mark || parity == parity_none || parity == parity_odd || parity == parity_space)) {
        return -1;
    }

    if (_parity != parity) {
        if (parity == parity_mark || (parity == parity_space && _databits == 8)) {
            return -1;
        } else {
            _parity = parity;
        }
    }

    if (!is_open()) {
        return 0;
    }

    switch (parity) {
    case parity_space:
        if (_databits == 8) {
            return -1;
        } else {
            /* space parity not directly supported - add an extra data bit to simulate it */
            _comm_config.c_cflag &= ~(PARENB | CSIZE);
            switch (_databits) {
            case 5:
                _databits = 6;
                _comm_config.c_cflag |= CS6;
                break;
            case 6:
                _databits = 7;
                _comm_config.c_cflag |= CS7;
                break;
            case 7:
                _databits = 8;
                _comm_config.c_cflag |= CS8;
                break;
            case 8:
                break;
            }
        }
        break;

    case parity_mark:
        return -1;
    case parity_none:
        _comm_config.c_cflag &= (~PARENB);
        break;
    case parity_even:
        _comm_config.c_cflag &= (~PARODD);
        _comm_config.c_cflag |= PARENB;
        break;
    case parity_odd:
        _comm_config.c_cflag |= (PARENB | PARODD);
        break;
    }
    if (_is_multi_setting)
        return 0;

    return tcsetattr(_fd, TCSAFLUSH, &_comm_config);
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
        _stopbits = stopbits;
        _comm_config.c_cflag &= (~CSTOPB);
        break;
    case stopbits_one_point_five:
        ec = boost::system::error_code(boost::system::errc::invalid_argument, boost::system::get_system_category());
        return ec;
        break;
    case stopbits_two:
        if (_databits == 5) {
            ec = boost::system::error_code(boost::system::errc::invalid_argument, boost::system::get_system_category());
            return ec;
        } else {
            _stopbits = stopbits;
            _comm_config.c_cflag |= CSTOPB;
        }
        break;
    }

    if (_is_multi_setting)
        return 0;

    return tcsetattr(_fd, TCSAFLUSH, &_comm_config);
}

int32 serial_port::set_flow_control(int32 flow_control) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!(flow_control == flow_control_hardware || flow_control == flow_control_xon_xoff || flow_control == flow_control_none)) {
        return -1;
    }

    if (_flow_control != flow_control) {
        _flow_control = flow_control;
    }

    if (!is_open()) {
        return 0;
    }

    switch (flow_control) {
    case flow_control_none:
        _comm_config.c_cflag &= (~CRTSCTS);
        _comm_config.c_iflag &= (~(IXON | IXOFF | IXANY));
        break;

    case flow_control_xon_xoff:
        _comm_config.c_cflag &= (~CRTSCTS);
        _comm_config.c_iflag |= (IXON | IXOFF | IXANY);
        break;

    case flow_control_hardware:
        _comm_config.c_cflag |= CRTSCTS;
        _comm_config.c_iflag &= (~(IXON | IXOFF | IXANY));
        break;
    }

    if (_is_multi_setting)
        return 0;

    return tcsetattr(_fd, TCSAFLUSH, &_comm_config);
}

int32 serial_port::set_read_timeout(int64 millic_seconds) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    _read_timeout = millic_seconds;

    if (!is_open()) {
        return 0;
    }

    _copy_timeout.tv_sec = millic_seconds / 1000;
    _copy_timeout.tv_usec = millic_seconds % 1000;

    if (millic_seconds == -1) {
        int error_code = fcntl(_fd, F_SETFL, O_NDELAY);
        if (error_code)
            return error_code;
    } else {
        /* O_SYNC should enable blocking write()
         * however this seems not working on Linux 2.6.21 (works on
         * OpenBSD 4.2)
         */
        int error_code = fcntl(_fd, F_SETFL, O_NDELAY);
        if (error_code)
            return error_code;
    }

    int error_code = tcgetattr(_fd, &_comm_config);
    if (error_code) {
        return error_code;
    }

    _comm.config.c_cc[VTIME] = millic_seconds / 100;

    if (_is_multi_setting)
        return 0;
    return tcsetattr(_fd, TCSAFLUSH, &_comm_config);
}

int32 serial_port::set_write_timeout(int64 millic_seconds) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    _write_timeout = millic_seconds;
    return 0;
}

bool serial_port::is_open() {
    return _fd != -1;
}

int32 serial_port::open() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    const long vdisable = fpathconf(_fd, _PC_VDISABLE);

    if (is_open()) {
        return -1;
    }
    log_inst().write(jm::core::log::debug, "serialport", std::string("trying to open file") + _port_name);

    /* note: linux 2.6.21 seems to ignore O_NDELAY flag */
    _fd = ::open(_port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (_fd == -1) {
        return -1;
    }

    log_inst().write(jm::core::log::debug, "serialport", "file opened successfully");

    /* Save the old termios */
    int error_code = tcgetattr(_fd, &_old_termios);
    if (error_code != 0) {
        return error_code;
    }

    _comm_config = _old_termios; /* Make a working copy */
    cfmakeraw(&_comm_config); /* Enable raw access */

    /* set up other port settings */
    _comm_config.c_cflag |= CREAD | CLOCAL;
    _comm_config.c_lflag &= (~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG));
    _comm_config.c_iflag &= (~(INPCK | IGNPAR | PARMRK | ISTRIP | ICRNL | IXANY));
    _comm_config.c_oflag &= (~OPOST);
    _comm_config.c_cc[VMIN] = 0;
#ifdef _POSIX_VDISABLE /* Is a disable character available on this system ? */
    /* Some systems allow for per-device disable-characters, so get the
     * proper value for the configured device.
     */
    _comm_config.c_cc[VINTR] = vdisable;
    _comm_config.c_cc[VQUIT] = vdisable;
    _comm_config.c_cc[VSTART] = vdisable;
    _comm_config.c_cc[VSTOP] = vdisable;
    _comm_config.c_cc[VSUSP] = vdisable;
#endif /* _POSIX_VDISABLE */
    _is_multi_setting = true;
    if (set_baudrate(_baudrate)
            || set_databits(_databits)
            || set_parity(_parity)
            || set_stopbits(_stopbits)
            || set_flow_control(_flow_control)
            || set_read_timeout(_read_timeout)
            || set_write_timeout(_write_timeout)) {
        _is_multi_setting = false;
        return -1;
    }

    int error_code = tcsetattr(_fd, TCSAFLUSH, &_comm_config);
    if (error_code)
        close();
    _is_multi_setting = false;
    return error_code;
}

int32 serial_port::close() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    log_inst().write(jm::core::log::debug, "serialport", "close");
    if (!is_open()) {
        return 0;
    }

    const long vdisable = fpathconf(_fd, _PC_VDISABLE);

    /* Force a flush */
    flush();

    /* Using both TCSAFLUSH and TCSANOW here discards any pending input */
    int error_code = tcsetattr(_fd, TCSAFLUSH | TCSANOW, &_old_termios); /* Restore termios */
    if (error_code != 0) {
        return error_code;
    }
    error_code = ::close(_fd);
    if (error_code != 0) {
        return error_code;
    }
    _fd = -1;
    return 0;
}

int32 serial_port::flush() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    return tcflush(_fd, TCIOFLUSH);
}

int32 serial_port::discard_in_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    return tcflush(_fd, TCIFLUSH);
}

int32 serial_port::discard_out_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    return tcflush(_fd, TCOFLUSH);
}

int64 serial_port::bytes_available() {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    int bytes_queued;

    int error_code = ioctl(_fd, FIONREAD, &bytes_queued);
    if (error_code != 0) {
        return -1;
    }

    return static_cast<int64> (bytes_queued);
}

int32 serial_port::set_dtr(bool set) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    int status;
    int error_code = ioctl(_fd, TIOCMGET, &status);
    if (error_code != 0) {
        return error_code;
    }

    if (set) {
        status |= TIOCM_DTR;
    } else {
        status &= ~TIOCM_DTR;
    }

    return ioctl(_fd, TIOCMSET, &status);
}

int32 serial_port::set_rts(bool set) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (!is_open()) {
        return -1;
    }

    int status;
    int error_code = ioctl(_fd, TIOCMGET, &status);
    if (error_code != 0) {
        return error_code;
    }
    if (set) {
        status |= TIOCM_RTS;
    } else {
        status &= ~TIOCM_RTS;
    }
    return ioctl(_fd, TIOCMSET, &status);
}

int32 serial_port::read(uint8 *data, int32 offset, int32 count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (data == NULL)
        return -1;

    if (count <= 0)
        return -1;

    if (!is_open())
        return -1;

    int ret_val = ::read(_fd, data + offset, count);
    return static_cast<int32>(ret_val);
}

int32 serial_port::write(const uint8 *data, int32 offset, int32 count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_mutex);
    if (data == NULL)
        return -1;

    if (count <= 0)
        return -1;

    if (!is_open())
        return -1;

    int ret_val = ::write(_fd, data + offset, count);
    return static_cast<int32>(ret_val);
}

std::vector<std::string> serial_port::get_system_ports() {
    std::vector<std::string> vec;
    struct dirent *ent = NULL;
    DIR *dir_p;
    char dir[512];
    struct stat statbuf;

    if ((dir_p = opendir("/dev")) == NULL) {
        return vec;
    }

    while ((ent = readdir(dir_p)) != NULL) {
        /* get file absolute name */
        snprintf(dir, 512, "%s/%s", "/dev", ent->d_name);
        /* get file information */
        lstat(dir, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            /* skip directory */
        } else {
            if ((ent->d_name[0] == 't') && (ent->d_name[1] == 't')
                    && (ent->d_name[2] == 'y')) {
                if ((ent->d_name[3] == 'S')
                        || ((ent->d_name[3] == 'U') && (ent->d_name[4] == 'S')
                        && (ent->d_name[5] == 'B'))
                        || ((ent->d_name[3] == 'A') && (ent->d_name[4] == 'C')
                        && (ent->d_name[5] == 'M'))) {
                    vec.push_back(ent->d_name);
                }
            } else if ((ent->d_name[0] == 'r') && (ent->d_name[1] == 'f')
                    && (ent->d_name[2] == 'c') && (ent->d_name[3] == 'o')
                    && (ent->d_name[4] == 'm') && (ent->d_name[5] == 'm')) {
                vec.push_back(ent->d_name);
            }
        }
    }
    return vec;
}

}

#endif