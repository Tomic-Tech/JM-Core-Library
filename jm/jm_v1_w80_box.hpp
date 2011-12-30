/* 
 * File:   jm_v1_w80.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月24日, 下午8:22
 */

#ifndef JM_V1_W80_BOX_HPP
#define	JM_V1_W80_BOX_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <jm_v1_box.hpp>
#include <jm_v1_w80_d.hpp>

namespace jm {
namespace v1 {
class v1_w80_box : public v1_box<v1_w80_d> {

private:
    int32 _serial_port_baud;
    uint8 _serial_port_databits;
    int32 _serial_port_stopbits;
    int32 _serial_port_flow_control;
    int32 _serial_port_parity;
    uint32 _box_time_unit;
    uint32 _time_base_db;
    uint32 _time_extern_db;
    uint8 _ports[D::MAXPORT_NUM];
    uint8 _buf[D::MAXBUFF_LEN];
    uint32 _pos;
    bool _is_link;
    uint32 _run_flag;
    uint32 _box_ver;
    uint32 _start_pos;
public:
    bool serial_port_change_config();
    bool check_serial_port_change_config();
    int32 serial_port_baud();
    uint8 serial_port_databits();
    int32 serial_port_parity();
    int32 serial_port_stopbits();
    int32 serial_port_flow_control();
    v1_w80_box(const commbox_port_ptr &port, const v1_shared_ptr &shared);
    bool check_result(int64 microseconds);
    size_t read_data(uint8 *buff, size_t offset, size_t count, int64 microseconds);
    size_t read_bytes(uint8 *buff, size_t offset, size_t count);
    uint8 get_cmd_data(uint8 *buff, size_t count);
    bool set_line_level(uint8 value_low, uint8 value_high);
    bool set_comm_ctrl(uint8 value_open, uint8 value_close);
    bool set_comm_line(uint8 send_line, uint8 recv_line);
    bool turn_over_one_by_one();
    bool keep_link(bool is_run_link);
    bool set_comm_link(uint8 ctrl_word1, uint8 ctrl_word2, uint8 ctrl_word3);
    bool set_comm_baud(double baud);
    void get_link_time(uint8 type, uint32 time);
    bool set_comm_time(uint8 type, uint32 time);
    bool commbox_delay(uint32 time);
    bool send_out_data(const uint8* buff, size_t offset, size_t count);
    bool run_receive(uint8 type);
    bool stop_now(bool is_stop_execute);
    bool open_comm();
    bool close_comm();
    uint8 get_abs_add(uint8 buff_id, uint8 add);
    bool update_buff(uint8 type, uint8 *buff);
    bool copy_buff(uint8 dest, uint8 src, uint8 len);
    bool new_batch(uint8 buff_id);
    bool end_batch();
    bool del_batch(uint8 buff_id);
    bool run_batch(uint8 *buff, size_t count, bool is_exeucte_many);
    bool reset();
private:
    bool check_idle();
    bool check_send();
    bool send_cmd(uint8 cmd, size_t offset, size_t count, const uint8 *buff);
    bool do_cmd(uint8 cmd, size_t offset, size_t count, const uint8 *buff);
    bool do_set(uint8 cmd, size_t offset, size_t count, const uint8 *buff);
    uint8 get_buff_data(uint8 addr, uint8 *buff, size_t count);
    bool init_box();
    bool check_box();
    bool open_box();
    bool set_rf(uint8 cmd, uint8 data);
    uint32 get_box_ver();
};

typedef boost::shared_ptr<v1_w80_box> v1_w80_box_ptr;

}
}


#endif	/* JM_V1_W80_BOX_HPP */

