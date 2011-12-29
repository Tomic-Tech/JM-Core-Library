#include "jm_commbox.hpp"
#include "jm_v1_c168_box.hpp"
#include "jm_v1_w80_box.hpp"

namespace jm {

commbox::commbox(const commbox_port_ptr &port) {
    _port = port;
}

commbox_port_ptr commbox::get_port() {
    return _port;
}

int32 commbox::serial_port_baud() {
    return 0;
}

uint8 commbox::serial_port_databits() {
    return 0;
}

int32 commbox::serial_port_parity() {
    return 0;
}

int32 commbox::serial_port_stopbits() {
    return 0;
}

int32 commbox::serial_port_flow_control() {
    return 0;
}

bool commbox::serial_port_change_config() {
    return false;
}

bool commbox::check_serial_port_change_config() {
    return false;
}

}
