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

#include <jm_port.hpp>
#include <jm_comm.hpp>
#include <jm_connector_type.hpp>
#include <jm_protocol_type.hpp>
#include <jm_serial_port.hpp>
#include <jm_commbox_error.hpp>
#include <boost/smart_ptr.hpp>

namespace jm {

class commbox {
public:
    commbox(const boost::shared_ptr<port> &port);
    virtual boost::shared_ptr<port> get_port();
    virtual int32 open() = 0;
    virtual int32 close() = 0;
    virtual void* configure(protocol_type type) = 0;
    virtual int32 set_connector(connector_type type) = 0;
    // this only for serial port configuration.
    virtual int32 serial_port_baud();
    virtual uint8 serial_port_databits();
    virtual int32 serial_port_parity();
    virtual int32 serial_port_stopbits();
    virtual int32 serial_port_flow_control();
    virtual bool serial_port_change_config();
    virtual bool check_serial_port_change_config();
private:
    boost::shared_ptr<port> _port;
};

}


#endif	/* JM_DEVICE_COMMBOX_HPP */

