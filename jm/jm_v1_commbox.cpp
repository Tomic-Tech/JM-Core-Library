#include "jm_v1_commbox.hpp"
#include "jm_v1_c168_box.hpp"
#include "jm_v1_w80_box.hpp"

namespace jm {
namespace v1 {

v1_commbox::v1_commbox(const commbox_port_ptr& port)
: commbox(port)
, _current_box(0) { // C168
    _shared = boost::make_shared<v1_shared > ();
    _box = boost::make_shared<v1_c168_box > (get_port(), _shared);
}

error_code v1_commbox::open() {

    if (_current_box == 0) {
        if (!_box->open_comm()) {
            _current_box = 1;
            _box = boost::make_shared<v1_w80_box > (get_port(), _shared);
            return error::commbox_try_again;
        }
    } else {
        if (!_box->open_comm()) {
            _current_box = 0;
            _box = boost::make_shared<v1_c168_box > (get_port(), _shared);
            return error::commbox_open_fail;
        }
    }
    return error::success;
}

error_code v1_commbox::close() {
    if (_box->close_comm())
        return error::success;
    return error::generic_error;
}

pointer v1_commbox::configure(protocol_type type) {
    return NULL;
}

error_code v1_commbox::set_connector(connector_type type) {
    _shared->connector = type;
    return error::success;
}

int32 v1_commbox::serial_port_baud() {
    return _box->serial_port_baud();
}

uint8 v1_commbox::serial_port_databits() {
    return _box->serial_port_databits();
}

int32 v1_commbox::serial_port_parity() {
    return _box->serial_port_parity();
}

int32 v1_commbox::serial_port_stopbits() {
    return _box->serial_port_stopbits();
}

int32 v1_commbox::serial_port_flow_control() {
    return _box->serial_port_flow_control();
}

bool v1_commbox::serial_port_change_config() {
    return _box->serial_port_change_config();
}

bool v1_commbox::check_serial_port_change_config() {
    return _box->check_serial_port_change_config();
}

}
}