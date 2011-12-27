/* 
 * File:   jm_v1_commbox.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月24日, 下午11:53
 */

#ifndef JM_V1_COMMBOX_HPP
#define	JM_V1_COMMBOX_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_commbox.hpp>
#include <jm_v1_box.hpp>
#include <jm_v1_shared.hpp>

namespace jm {

class v1_commbox : public commbox {
public:
    v1_commbox(const boost::shared_ptr<port> &port);
    int32 open();
    int32 close();
    void* configure(protocol_type type);
    int32 set_connector(connector_type type);
    // this only for serial port configuration.
    int32 serial_port_baud();
    uint8 serial_port_databits();
    int32 serial_port_parity();
    int32 serial_port_stopbits();
    int32 serial_port_flow_control();
    bool serial_port_change_config();
    bool check_serial_port_change_config();
private:
    boost::shared_ptr<v1_box> _box;
    boost::shared_ptr<v1_shared> _shared;
    uint32 _current_box;
};

}


#endif	/* JM_V1_COMMBOX_HPP */

