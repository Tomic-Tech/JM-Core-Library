/* 
 * File:   jm_serial_port.cpp
 * Author: Ogilvy
 * 
 * Created on 2011年12月15日, 下午10:40
 */

#include "jm_serial_port.hpp"
#include "jm_serial_port_private.hpp"
#include <boost/thread.hpp>

namespace jm {

serial_port::serial_port() {
	_pri = new serial_port_private;
    initialize();
    platform_initialize();
}

serial_port::serial_port(const std::string &name) {
	_pri = new serial_port_private;
    initialize();
    platform_initialize();
    set_port_name(name);
}

void serial_port::initialize() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    _pri->_read_timeout = 500;
    _pri->_write_timeout = 500;
    _pri->_parity = parity_none;
    _pri->_stopbits = stopbits_one;
    _pri->_flow_control = flow_control_none;
    _pri->_baudrate = 57600;
    _pri->_databits = 8;
    _pri->_is_multi_setting = false;
}

serial_port::~serial_port() {
    if (!is_open()) {
        platform_destroy();
        return;
    }
    close();
    platform_destroy();
}

std::string serial_port::port_name() const {
    return _pri->_port_name;
}

int32 serial_port::baudrate() const {
    return _pri->_baudrate;
}

uint8 serial_port::databits() const {
    return _pri->_databits;
}

int32 serial_port::stopbits() const {
    return _pri->_stopbits;
}

int32 serial_port::parity() const {
    return _pri->_parity;
}

int32 serial_port::flow_control() const {
    return _pri->_flow_control;
}

int64 serial_port::read_timeout() const {
    return _pri->_read_timeout;
}

int64 serial_port::write_timeout() const {
    return _pri->_write_timeout;
}

}

