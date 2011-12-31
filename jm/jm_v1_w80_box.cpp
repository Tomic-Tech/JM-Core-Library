#include "jm_v1_w80_box.hpp"
#include "jm_serial_port.hpp"
#include "jm_byte_array.hpp"
#include "jm_utils.hpp"

namespace jm {
namespace v1 {
v1_w80_box::v1_w80_box(const commbox_port_ptr &port, const v1_shared_ptr &shared)
: v1_box(port, shared)
, _serial_port_baud(115200)
, _serial_port_stopbits(serial_port::stopbits_two)
, _serial_port_databits(8)
, _serial_port_flow_control(serial_port::flow_control_none)
, _serial_port_parity(serial_port::parity_none)
, _is_initialize(true) {

}

bool v1_w80_box::check_idle() {
    uint8 rb = D::READY;
    int32 avail = get_port()->bytes_available();
    if (avail > 20) {
        get_port()->discard_in_buffer();
        return true;
    }
    while (get_port()->bytes_available()) {
        get_port()->read(&rb, 0, 1);
    }
//    while (get_port()->read(&rb, 0, 1));
    if (rb == D::READY || rb == D::Error) {
        return true;
    }
    if (get_port()->read(&rb, 0, 1) != 1) {
        return false;
    }
    if (rb == D::READY || rb == D::Error) {
        return true;
    }
    return false;
}

bool v1_w80_box::check_send() {
    uint8 rb = 0;
    if (get_port()->read(&rb, 0, 1) != 1) {
        return false;
    }
    if (rb == D::RECV_OK) {
        return true;
    }
}

bool v1_w80_box::check_result(int64 microseconds) {
    uint8 rb = 0;
    if (get_port()->read(&rb, 0, 1) != 1) {
        return false;
    }
    if (rb == D::READY || rb == D::Error) {
        get_port()->discard_in_buffer();
        return true;
    }
    return false;
}

bool v1_w80_box::send_cmd(uint8 cmd, size_t offset, size_t count, const uint8 *data) {
    uint8 cs = cmd;
    cmd += _run_flag;
    byte_array command;
    command.push_back(cmd);
    for (int i = 0; i < count; i++) {
        command.push_back(data[i + offset]);
        cs += data[i + offset];
    }
    command.push_back(cs);
    for (int32 i = 0; i < 3; i++) {
        if (!check_idle()) {
            continue;
        }
        if (get_port()->write(command.data(), 0, command.size()) != command.size()) {
            continue;
        }
        if (check_send()) {
            return true;
        }
    }
    return false;
}

size_t v1_w80_box::read_data(uint8 *buff, size_t offset, size_t count, int64 microseconds) {
    get_port()->set_read_timeout(microseconds);
    if (get_port()->read(buff, offset, count) != count) {
        int32 avail = get_port()->bytes_available();
        if (avail > 0) {
            if (avail <= count) {
                return get_port()->read(buff, offset, avail);
            } else {
                return get_port()->read(buff, offset, count);
            }
        }
        return avail;
    }
    return count;
}

size_t v1_w80_box::read_bytes(uint8 *buff, size_t offset, size_t count) {
    return read_data(buff, offset, count, timer::to_ms(500));
}

uint8 v1_w80_box::get_cmd_data(uint8 *receive_buffer, size_t count) {
    uint8 len = 0;
    uint8 cs = 0;
    if ((read_bytes(receive_buffer, 0, 1) != 1)
            || (read_bytes(&len, 0, 1) != 1)) {
        return 0;
    }
    if (len > count) {
        len = count;
    }
    if (read_bytes(receive_buffer, 0, len) != len) {
        return 0;
    }
    if (read_bytes(&cs, 0, 1) != 1) {
        return 0;
    }
    return len;
}

bool v1_w80_box::do_cmd(uint8 cmd, size_t offset, size_t count, const uint8 *buff) {
    _start_pos = 0;
    if (cmd != D::WR_DATA && cmd != D::SEND_DATA) {
        cmd |= count; //加上长度位
    }
    if (get_shared()->is_do_now) {
        //发送到BOX执行
        if (cmd == D::WR_DATA) {
            byte_array temp;
            if (count == 0)
                return false;
            if (_is_link) {
                temp.push_back(0xFF); //写链路保持
            } else {
                temp.push_back(0); //写通讯命令
            }
            temp.push_back(count);
            temp.push_back(buff, offset, count);
            return send_cmd(D::WR_DATA, 0, temp.size(), temp.data());
        } else if (cmd == D::SEND_DATA) {
            if (count == 0)
                return false;
            byte_array temp;
            temp.push_back(0); //写入位置
            temp.push_back(count + 2); //数据包长度
            temp.push_back(D::SEND_DATA); //命令
            temp.push_back(count - 1); //命令长度-1
            temp.push_back(buff, offset, count);
            if (!send_cmd(D::WR_DATA, 0, temp.size(), temp.data()))
                return false;
            return send_cmd(D::DO_BAT_C, 0, 0, NULL);
        } else {
            return send_cmd(cmd, offset, count, buff);
        }
    } else {
        //写命令到缓冲区
        _buf[_pos++] = cmd;
        if (cmd == D::SEND_DATA) {
            _buf[_pos++] = count - 1;
        }
        _start_pos = _pos;
        if (count > 0) {
            memcpy(_buf + _pos, buff + offset, count);
        }
        _pos += count;
        return true;
    }
}

bool v1_w80_box::do_set(uint8 cmd, size_t offset, size_t count, const uint8 *buff) {
    bool result = do_cmd(cmd, offset, count, buff);
    if (result && get_shared()->is_do_now) {
        result = check_result(timer::to_ms(150));
    }
    return result;
}

uint8 v1_w80_box::get_buff_data(uint8 addr, uint8 *buff, size_t count) {
    uint8 temp[2];
    temp[0] = addr;
    temp[1] = count;
    if (!do_cmd(D::GET_BUF, 0, 2, temp)) {
        return 0;
    }
    return get_cmd_data(buff, count);
}

bool v1_w80_box::init_box() {
    static uint8 password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 0x17, 0x2D, 0x4D};
    srand((unsigned char) time(NULL));
    get_shared()->is_do_now = true;
    _run_flag = 0;
    uint8 buf[32];
    for (int32 i = 0; i < 4; i++) {
        buf[i] = low_byte(rand());
    }
    uint8 run = 0;
    for (int32 i = 0; i < 10; i++) {
        run += password[i] ^ buf[i % 3 + 1];
    }
    if (run == 0) {
        run = 0x55;
    }
    if (!do_cmd(D::GET_CPU, 1, 3, buf)) {
        return false;
    }
    if (get_cmd_data(buf, 32) <= 0) {
        return false;
    }
    _run_flag = 0; // Run
    _box_time_unit = 0;
    int32 i = 0;
    for (; i < 3; i++) {
        _box_time_unit = _box_time_unit * 256 + buf[i];
    }
    _time_base_db = buf[i++];
    _time_extern_db = buf[i++];

    for (i = 0; i < D::MAXPORT_NUM; i++) {
        _ports[i] = 0xFF;
    }
    _pos = 0;
    get_shared()->is_db_20 = false;
    return true;
}

bool v1_w80_box::check_box() {
    uint8 buff[32];
    if (!do_cmd(D::GET_BOXID, 0, 0, NULL)) {
        return false;
    }
    if (get_cmd_data(buff, 32) <= 0) {
        return false;
    }
    _box_ver = (buff[10] << 8) | buff[11];
    return true;
}

bool v1_w80_box::set_line_level(uint8 value_low, uint8 value_high) {
    _ports[1] &= ~value_low;
    _ports[1] |= value_high;
    return do_set(D::SET_PORT1, 1, 1, _ports);
}

bool v1_w80_box::set_comm_ctrl(uint8 value_open, uint8 value_close) {
    _ports[2] &= ~value_open;
    _ports[2] |= value_close;
    return do_set(D::SET_PORT2, 2, 1, _ports);
}

bool v1_w80_box::set_comm_line(uint8 send_line, uint8 recv_line) {
    if (send_line > 7) {
        send_line = 0x0F;
    }
    if (recv_line > 7) {
        recv_line = 0x0F;
    }
    _ports[0] = send_line | (recv_line << 4);
    return do_set(D::SET_PORT0, 0, 1, _ports);
}

bool v1_w80_box::turn_over_one_by_one() {
    return do_set(D::SET_ONEBYONE, 0, 0, NULL);
}

bool v1_w80_box::keep_link(bool is_run_link) {
    if (is_run_link) {
        return do_set(D::RUN_LINK, 0, 0, NULL);
    } else {
        return do_set(D::STOP_LINK, 0, 0, NULL);
    }
}

bool v1_w80_box::set_comm_link(uint8 ctrl_word1, uint8 ctrl_word2, uint8 ctrl_word3) {
    uint8 ctrl_word[3];
    uint8 mode_control = ctrl_word1 & 0xE0;
    int32 length = 3;
    ctrl_word[0] = ctrl_word1;
    if ((ctrl_word1 & 0x04) != 0) {
        get_shared()->is_db_20 = true;
    } else {
        get_shared()->is_db_20 = false;
    }
    if (mode_control == D::SET_VPW || mode_control == D::SET_PWM) {
        return do_set(D::SET_CTRL, 0, 1, ctrl_word);
    }
    ctrl_word[1] = ctrl_word2;
    ctrl_word[2] = ctrl_word3;
    if (ctrl_word3 == 0) {
        length--;
        if (ctrl_word2 == 0) {
            length--;
        }
    }
    if (mode_control == D::EXRS_232 && length < 2) {
        return false;
    }
    return do_set(D::SET_CTRL, 0, length, ctrl_word);
}

bool v1_w80_box::set_comm_baud(double baud) {
    uint8 baud_time[2];
    double instruct_num = ((1000000.0 / _box_time_unit) * 1000000) / baud;
    if (get_shared()->is_db_20) {
        instruct_num /= 20;
    }
    instruct_num += 0.5;
    if (instruct_num > 65535 || instruct_num < 10) {
        return false;
    }
    baud_time[0] = high_byte(instruct_num);
    baud_time[1] = low_byte(instruct_num);
    if (baud_time[0] == 0) {
        return do_set(D::SET_BAUD, 1, 1, baud_time);
    }
    return do_set(D::SET_BAUD, 0, 2, baud_time);
}

void v1_w80_box::get_link_time(uint8 type, uint32 time) {
    if (type == D::SETBYTETIME) {
        get_shared()->req_byte_to_byte = time;
    } else if (type == D::SETRECBBOUT) {
        get_shared()->req_wait_time = time;
    } else if (type == D::SETRECBBOUT) {
        get_shared()->res_byte_to_byte = time;
    } else {
        get_shared()->res_wait_time = time;
    }
}

bool v1_w80_box::set_comm_time(uint8 type, uint32 time) {
    uint8 time_buff[2];
    get_link_time(type, time);
    if (type == D::SETVPWSTART || type == D::SETVPWRECS) {
        if (type == D::SETVPWRECS) {
            time = (time * 2) / 3;
        }
        type = type + (D::SETBYTETIME & 0xF0);
        time = static_cast<uint32> (time / (_box_time_unit / 1000000.0));
    } else {
        time = static_cast<uint32> ((time / _time_base_db) / (_box_time_unit / 1000000.0));
    }
    time_buff[0] = high_byte(time);
    time_buff[1] = low_byte(time);
    if (time_buff[0] == 0) {
        return do_set(type, 1, 1, time_buff);
    }
    return do_set(type, 0, 2, time_buff);
}

bool v1_w80_box::commbox_delay(uint32 time) {
    uint8 time_buff[2];
    uint8 delay_word = D::DELAYSHORT;
    time = static_cast<uint32> (time / (_box_time_unit / 1000000.0));
    if (time == 0) {
        return false;
    }
    if (time > 65535) {
        time = static_cast<uint32> (time / _time_base_db);
        if (time > 65535) {
            time = static_cast<uint32> ((time * _time_base_db) / _time_extern_db);
            if (time > 65535) {
                return false;
            }
            delay_word = D::DELAYDWORD;
        } else {
            delay_word = D::DELAYTIME;
        }
    }
    time_buff[0] = high_byte(time);
    time_buff[1] = low_byte(time);
    if (time_buff[0] == 0) {
        return do_set(delay_word, 1, 1, time_buff);
    }
    return do_set(delay_word, 0, 2, time_buff);
}

bool v1_w80_box::send_out_data(const uint8 *buffer, size_t offset, size_t count) {
    return do_set(D::SEND_DATA, offset, count, buffer);
}

bool v1_w80_box::run_receive(uint8 type) {
    if (type == D::GET_PORT1) {
        get_shared()->is_db_20 = false;
    }
    return do_cmd(type, 0, 0, NULL);
}

bool v1_w80_box::stop_now(bool is_stop_execute) {
    uint8 cmd;
    if (is_stop_execute) {
        cmd = D::STOP_EXECUTE;
    } else {
        cmd = D::STOP_REC;
    }
    for (int i = 0; i < 3; i++) {
        if (get_port()->write(&cmd, 0, 1) != 1) {
            continue;
        }
        if (check_send()) {
            if (is_stop_execute && !check_result(timer::to_ms(200))) {
                continue;
            }
            return true;
        }
    }
    return false;
}

bool v1_w80_box::set_rf(uint8 cmd, uint8 data) {
    return false;
}

bool v1_w80_box::open_box() {
    if (init_box() && check_box()) {
        get_port()->discard_in_buffer();
        get_port()->discard_out_buffer();
        _is_initialize = false;
        return true;
    }
    return false;
}

bool v1_w80_box::open_comm() {
    _is_initialize = true;
    return open_box();
}

bool v1_w80_box::close_comm() {
    reset();
    set_rf(D::RF_RESET, 0);
    _is_initialize = false;
    return true;
}

uint8 v1_w80_box::get_abs_add(uint8 buff_id, uint8 add) {
    return 0;
}

// buffer[0] = addr, buffer[1] = data

bool v1_w80_box::update_buff(uint8 type, uint8 *buffer) {
    int len = 0;
    uint8 buf[3];
    buf[0] = buffer[0];
    buf[1] = buffer[1];
    if ((type == D::INC_BYTE) || (type == D::DEC_BYTE) || (type == D::INVERTBYTE)) {
        len = 1;
    } else if ((type == D::UPDATE_BYTE) || (type == D::ADD_BYTE) || (type == D::SUB_BYTE)) {
        len = 2;
    } else {
        len = 3;
    }
    return do_set(type, 0, len, buf);
}

bool v1_w80_box::copy_buff(uint8 dest, uint8 src, uint8 len) {
    uint8 buf[3];
    buf[0] = dest;
    buf[1] = src;
    buf[2] = len;
    return do_set(D::COPY_BYTE, 0, 3, buf);
}

bool v1_w80_box::new_batch(uint8 buff_id) {
    _pos = 0;
    _is_link = (buff_id == D::LINKBLOCK ? true : false);
    get_shared()->is_do_now = false;
    return true;
}

bool v1_w80_box::end_batch() {
    int i = 0;
    get_shared()->is_do_now = true;
    _buf[_pos++] = 0; //命令块以0x00标记结束
    if (_is_link) { //修改update_buff使用到的地址
        while (_buf[i] != 0) {
            uint8 mode = _buf[i] & 0xFC;
            if (mode == D::COPY_BYTE) {
                _buf[i + 3] = _buf[i + 3] + D::MAXBUFF_LEN - _pos;
            } else if (mode == D::SUB_BYTE) {
                _buf[i + 2] = _buf[i + 2] + D::MAXBUFF_LEN - _pos;
            } else {
                _buf[i + 1] = _buf[i + 1] + D::MAXBUFF_LEN - _pos;

            }
            if (_buf[i] == D::SEND_DATA) {
                i += 1 + _buf[i + 1] + 1 + 1;
            } else if (_buf[i] >= D::REC_LEN_1 && _buf[i] <= D::REC_LEN_15) {
                i++; //特殊
            } else {
                i = i + ((_buf[i] & 0x03) + 1);
            }
        }
    }
    return do_cmd(D::WR_DATA, 0, _pos, _buf);
}

bool v1_w80_box::del_batch(uint8 buff_id) {
    get_shared()->is_do_now = true;
    _pos = 0;
    return true;
}

bool v1_w80_box::run_batch(uint8 *buff_id, size_t count, bool is_execute_many) {
    int cmd;
    if (buff_id[0] == D::LINKBLOCK) {
        if (is_execute_many) {
            cmd = D::DO_BAT_LN;
        } else {
            cmd = D::DO_BAT_L;
        }
    } else {
        if (is_execute_many) {
            cmd = D::DO_BAT_CN;
        } else {
            cmd = D::DO_BAT_C;
        }
    }
    return do_cmd(cmd, 0, 0, NULL);
}

bool v1_w80_box::reset() {
    stop_now(true);
    get_port()->discard_in_buffer();
    get_port()->discard_out_buffer();
    for (int i = 0; i < D::MAXPORT_NUM; i++) {
        _ports[i] = (byte) (0xFF);
    }
    return do_cmd(D::RESET, 0, 0, NULL);
}

uint32 v1_w80_box::get_box_ver() {
    return _box_ver;
}

int32 v1_w80_box::serial_port_baud() {
    return _serial_port_baud;
}

uint8 v1_w80_box::serial_port_databits() {
    return _serial_port_databits;
}

int32 v1_w80_box::serial_port_parity() {
    return _serial_port_parity;
}

int32 v1_w80_box::serial_port_stopbits() {
    return _serial_port_stopbits;
}

int32 v1_w80_box::serial_port_flow_control() {
    return _serial_port_flow_control;
}

bool v1_w80_box::serial_port_change_config() {
    if (_is_initialize)
        return true;
    return false;
}

bool v1_w80_box::check_serial_port_change_config() {
    return true;
}
}
}

