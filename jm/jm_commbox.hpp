/* 
 * File:   jm_device_commbox.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午1:58
 */

#ifndef JM_COMMBOX_HPP
#define	JM_COMMBOX_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <jm/jm_commbox_port.hpp>
#include <jm/jm_comm.hpp>
#include <jm/jm_connector_type.hpp>
#include <jm/jm_protocol_type.hpp>
#include <jm/jm_serial_port.hpp>
#include <jm/jm_error.hpp>

namespace jm {

class JMCORE_API commbox {
public:
    commbox(const commbox_port_ptr &port);
    virtual commbox_port_ptr get_port();
    virtual error_code open() = 0;
    virtual error_code close() = 0;
    virtual pointer configure(protocol_type type) = 0;
    virtual error_code set_connector(connector_type type) = 0;
    // this only for serial port configuration.
    virtual int32 serial_port_baud();
    virtual uint8 serial_port_databits();
    virtual int32 serial_port_parity();
    virtual int32 serial_port_stopbits();
    virtual int32 serial_port_flow_control();
    virtual bool serial_port_change_config();
    virtual bool check_serial_port_change_config();
private:
    commbox_port_ptr _port;
};

typedef boost::shared_ptr<commbox> commbox_ptr;

}


#endif	/* JM_DEVICE_COMMBOX_HPP */

