/* 
 * File:   jm_box.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午4:26
 */

#ifndef JM_V1_BOX_HPP
#define	JM_V1_BOX_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <jm_commbox_port.hpp>
#include <jm_v1_shared.hpp>

namespace jm {
namespace v1 {

class v1_box_base {
private:
    commbox_port_ptr _port;
    v1_shared_ptr _shared;
public:

    v1_box_base(const commbox_port_ptr &port, const v1_shared_ptr &shared) {
        _port = port;
        _shared = shared;
    }

    commbox_port_ptr get_port() {
        return _port;
    }

    v1_shared_ptr get_shared() {
        return _shared;
    }
    virtual bool open_comm() = 0;
    virtual bool close_comm() = 0;
    virtual size_t read_bytes(uint8 *buff, size_t offset, size_t count) = 0;
    virtual bool set_comm_ctrl(uint8 value_open, uint8 value_close) = 0;
    virtual bool set_comm_line(uint8 send_line, uint8 recv_line) = 0;
    virtual bool set_comm_link(uint8 ctrl_word1, uint8 ctrl_word2, uint8 ctrl_word3) = 0;
    virtual bool set_comm_baud(double baud) = 0;
    virtual bool set_comm_time(uint8 type, uint32 time) = 0;
    virtual bool set_line_level(uint8 value_low, uint8 value_high) = 0;
    virtual bool commbox_delay(uint32 time) = 0;
    virtual bool turn_over_one_by_one() = 0;
    virtual bool stop_now(bool is_stop_execute) = 0;
    virtual bool new_batch(uint8 buff_id) = 0;
    virtual bool del_batch(uint8 buff_id) = 0;
    virtual bool check_result(int64 microseconds) = 0;
    virtual bool keep_link(bool is_run_link) = 0;
    virtual bool send_out_data(const uint8 *buff, size_t offset, size_t count) = 0;
    virtual bool run_receive(uint8 type) = 0;
    virtual bool end_batch() = 0;
    virtual bool run_batch(uint8 *buff_id, size_t count, bool is_execute_many) = 0;
    virtual size_t read_data(uint8 *buff, int32 size_t, int32 size_t, int64 microseconds) = 0;
    virtual bool update_buff(uint8 type, uint8 *buff) = 0;
    virtual uint8 get_abs_add(uint8 buff_id, uint8 add) = 0;
    virtual int32 serial_port_baud() = 0;
    virtual uint8 serial_port_databits() = 0;
    virtual int32 serial_port_parity() = 0;
    virtual int32 serial_port_stopbits() = 0;
    virtual int32 serial_port_flow_control() = 0;
    virtual bool serial_port_change_config() = 0;
    virtual bool check_serial_port_change_config() = 0;
    virtual uint8 pwc() = 0;
    virtual uint8 rzfc() = 0;
    virtual uint8 ck() = 0;
    virtual uint8 set_null() = 0;
    virtual uint8 set_byte_time() = 0;
    virtual uint8 set_wait_time() = 0;
    virtual uint8 set_rec_bb_out() = 0;
    virtual uint8 set_rec_fr_out() = 0;
    virtual uint8 set_link_time() = 0;
    virtual uint8 rs_232() = 0;
    virtual uint8 bit9_mark() = 0;
    virtual uint8 sel_sl() = 0;
    virtual uint8 un_db20() = 0;
    virtual uint8 coms() = 0;
    virtual uint8 rec_fr() = 0;
};

template <typename Define>
class v1_box : public v1_box_base {
public:
    typedef Define D;
public:
    uint8 rec_fr() {
        return D::REC_FR;
    }
    uint8 coms() {
        return D::COMS;
    }
    uint8 pwc() {
        return D::PWC;
    }

    uint8 rzfc() {
        return D::RZFC;
    }

    uint8 ck() {
        return D::CK;
    }

    uint8 set_null() {
        return D::SET_NULL;
    }

    uint8 set_byte_time() {
        return D::SETBYTETIME;
    }

    uint8 set_wait_time() {
        return D::SETWAITTIME;
    }

    uint8 set_rec_bb_out() {
        return D::SETRECBBOUT;
    }

    uint8 set_rec_fr_out() {
        return D::SETRECFROUT;
    }

    uint8 set_link_time() {
        return D::SETLINKTIME;
    }

    uint8 rs_232() {
        return D::RS_232;
    }

    uint8 bit9_mark() {
        return D::BIT9_MARK;
    }

    uint8 sel_sl() {
        return D::SEL_SL;
    }

    uint8 un_db20() {
        return D::UN_DB20;
    }

    v1_box(const commbox_port_ptr &port, const v1_shared_ptr &shared)
    : v1_box_base(port, shared) {

    }
};

typedef boost::shared_ptr<v1_box_base> v1_box_ptr;

}
}

#endif	/* JM_BOX_HPP */

