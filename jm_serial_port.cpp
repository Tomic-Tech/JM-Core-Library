/* 
 * File:   jm_serial_port.cpp
 * Author: Ogilvy
 * 
 * Created on 2011年12月15日, 下午10:40
 */

#include "jm_serial_port.hpp"

namespace jm {

serial_port::serial_port() {
    initialize();
    platform_initialize();
}

serial_port::serial_port(const std::string &name) {
    initialize();
    platform_initialize();
    set_port_name(name);
}

void serial_port::initialize() {
    _read_timeout = 500;
    _write_timeout = 500;
    _parity = parity_none;
    _stopbits = stopbits_one;
    _flow_control = flow_control_none;
    _baudrate = 57600;
    _databits = 8;
    _is_multi_setting = false;
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
    return _port_name;
}

int32 serial_port::baudrate() const {
    return _baudrate;
}

uint8 serial_port::databits() const {
    return _databits;
}

int32 serial_port::stopbits() const {
    return _stopbits;
}

int32 serial_port::parity() const {
    return _parity;
}

int32 serial_port::flow_control() const {
    return _flow_control;
}

int64 serial_port::read_timeout() const {
    return _read_timeout;
}

int64 serial_port::write_timeout() const {
    return _write_timeout;
}

}

