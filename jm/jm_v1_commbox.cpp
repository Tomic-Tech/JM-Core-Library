#include "jm_v1_commbox.hpp"
#include "jm_v1_iso14230.hpp"
#include "jm_v1_comm.hpp"


namespace jm {
namespace v1 {

v1_commbox::v1_commbox(const commbox_port_ptr& port)
: commbox(port)
, _shared(boost::make_shared<v1_shared>())
, _c168_box(boost::make_shared<v1_c168_box>(port, _shared))
, _w80_box(boost::make_shared<v1_w80_box>(port, _shared))
, _current_box(0) { // C168
}

error_code v1_commbox::open() {

    if (_current_box == 0) {
        if (!_c168_box->open_comm()) {
            _current_box = 1;
            return error::commbox_try_again;
        }
    } else {
        if (!_w80_box->open_comm()) {
            _current_box = 0;
            return error::commbox_open_fail;
        }
    }
    return error::success;
}

error_code v1_commbox::close() {
    if (_current_box == 0) {
        if (_c168_box->close_comm())
            return error::success;
    } else {
        if (_w80_box->close_comm())
            return error::success;
    }
    return error::generic_error;
}

pointer v1_commbox::configure(protocol_type type) {
    pointer comm;
    if (_current_box == 0) {
        // C168
        switch (type) {
        case ISO14230:
        {
            boost::shared_ptr<v1_iso14230<v1_c168_box> > pro = boost::make_shared<v1_iso14230<v1_c168_box> >(_c168_box, _shared);
            comm = new v1_comm<v1_c168_box, v1_iso14230<v1_c168_box> >(_c168_box, pro);
            return comm;
        }
        default:
            return NULL;
        }
    } else {
        // W80
        switch (type) {
        case ISO14230:
        {
            boost::shared_ptr<v1_iso14230<v1_w80_box> > pro = boost::make_shared<v1_iso14230<v1_w80_box> >(_w80_box, _shared);
            comm = new v1_comm<v1_w80_box, v1_iso14230<v1_w80_box> >(_w80_box, pro);
            return comm;
        }
        default:
            return NULL;
        }
    }
    return NULL;
}

error_code v1_commbox::set_connector(connector_type type) {
    _shared->connector = type;
    return error::success;
}

int32 v1_commbox::serial_port_baud() {
    if (_current_box == 0) {
        return _c168_box->serial_port_baud();
    } else {
        return _w80_box->serial_port_baud();
    }

}

uint8 v1_commbox::serial_port_databits() {
    if (_current_box == 0) {
        return _c168_box->serial_port_databits();
    } else {
        return _w80_box->serial_port_databits();
    }
}

int32 v1_commbox::serial_port_parity() {
    if (_current_box == 0) {
        return _c168_box->serial_port_parity();
    } else {
        return _w80_box->serial_port_parity();
    }
}

int32 v1_commbox::serial_port_stopbits() {
    if (_current_box == 0) {
        return _c168_box->serial_port_stopbits();
    } else {
        return _w80_box->serial_port_stopbits();
    }

}

int32 v1_commbox::serial_port_flow_control() {
    if (_current_box == 0) {
        return _c168_box->serial_port_flow_control();
    } else {
        return _w80_box->serial_port_flow_control();
    }
}

bool v1_commbox::serial_port_change_config() {
    if (_current_box == 0) {
        return _c168_box->serial_port_change_config();
    } else {
        return _w80_box->serial_port_change_config();
    }
}

bool v1_commbox::check_serial_port_change_config() {
    if (_current_box == 0) {
        return _c168_box->check_serial_port_change_config();
    } else {
        return _w80_box->check_serial_port_change_config();
    }
}

}
}