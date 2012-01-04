#include "jm_v1_c168_box.hpp"
#include "jm_timer.hpp"
#include "jm_serial_port.hpp"
#include "jm_byte_array.hpp"
#include "jm_utils.hpp"

namespace jm {
namespace v1 {

v1_c168_box::v1_c168_box(const commbox_port_ptr &p, const v1_shared_ptr &s)
: v1_box(p, s)
, _serial_port_baud(9600)
, _serial_port_stopbits(serial_port::stopbits_one)
, _serial_port_databits(8)
, _serial_port_flow_control(serial_port::flow_control_none)
, _serial_port_parity(serial_port::parity_none)
, _is_initialize(true) {

}

bool v1_c168_box::check_idle() {
    uint8 receive_buffer = D::SUCCESS;
    int32 avail = get_port()->bytes_available();
    if (avail > 240) {
        get_port()->discard_in_buffer();
        get_port()->discard_out_buffer();
        return true;
    }
    while (avail) {
        get_port()->read(&receive_buffer, 0, 1);
        avail--;
    }
    if (receive_buffer == D::SUCCESS) {
        return true;
    }
    get_port()->set_read_timeout(timer::to_ms(200));
    if (get_port()->read(&receive_buffer, 0, 1) != 1) {
        get_shared()->last_error = D::KEEPLINK_ERROR;
        return false;
    }
    if (receive_buffer == D::SUCCESS) {
        return true;
    }
    get_shared()->last_error = receive_buffer;
    return false;
}

bool v1_c168_box::send_ok(int64 microseconds) {
    uint8 receive_buffer = 0;
    get_port()->set_read_timeout(microseconds);
    while (true) {
        if (get_port()->read(&receive_buffer, 0, 1) != 1) {
            get_shared()->last_error = D::TIMEOUT_ERROR;
            return false;
        }
        if (receive_buffer == D::SEND_OK) {
            return true;
        } else if (receive_buffer >= D::UP_TIMEOUT && receive_buffer <= D::ERROR_REC) {
            get_shared()->last_error = D::SENDDATA_ERROR;
            return false;
        }
    }

    get_shared()->last_error = D::SENDDATA_ERROR;
    return false;
}

uint32 v1_c168_box::get_box_ver() {
    return (_info.version[0] << 8) | (_info.version[1]);
}

bool v1_c168_box::commbox_do(uint8 command_word, size_type offset, size_type count, const uint8 *buff) {
    if (count > D::CMD_DATALEN) {
        return send_data_to_ecu(command_word, offset, count, buff);
    } else {
        return commbox_command(command_word, offset, count, buff);
    }
}

bool v1_c168_box::send_data_to_ecu(uint8 command_word, size_type offset, size_type count, const uint8 *buff) {
    if (command_word == D::SEND_DATA && count <= D::SEND_LEN) {
        if (_buffer.size() < (count + 1)) {
            get_shared()->last_error = D::NOBUFF_TOSEND;
            return false;
        }
        if (get_box_ver() > 0x400) {
            // 增加发送长命令
            if (!send_data_to_ecu_new(command_word, offset, count, buff)) {
                return false;
            }
        } else {
            // 保持与旧盒子兼容
            if (!send_data_to_ecu_old(command_word, offset, count, buff)) {
                return false;
            }
        }
        return commbox_do(D::D0_BAT, 0, 1, &_buffer.add[D::SWAPBLOCK]);
    }
    get_shared()->last_error = D::ILLIGICAL_LEN;
    return false;
}

bool v1_c168_box::send_data_to_ecu_new(uint8 command_word, size_type offset, size_type count, const uint8 *buff) {
    size_type i;
    byte_array command;
    command.push_back(D::WR_DATA + _info.head_password);
    command.push_back(low_byte(count + 3));
    command.push_back(_buffer.add[D::SWAPBLOCK]);
    command.push_back(D::SEND_CMD);
    command.push_back(low_byte(count - 1));
    uint8 checksum = D::WR_DATA + command[1] + command[2] + command[3] + command[4];

    for (i = 0; i < count; i++) {
        command.push_back(buff[offset + i]);
        checksum += buff[offset + i];
    }
    command.push_back(checksum);
    for (i = 0; i < 3; i++) {
        if (!check_idle() || (get_port()->write(command.data(), 0, command.size()) != command.size())) {
            get_shared()->last_error = D::SENDDATA_ERROR;
            continue;
        }
        if (send_ok(20 * (command.size() + 7))) {
            return true;
        }
    }
    return false;
}

bool v1_c168_box::send_data_to_ecu_old(uint8 command_word, size_type offset, size_type count, const uint8 *buff) {
    byte_array command;
    command.push_back(D::WR_DATA + _info.head_password);
    command.push_back(low_byte(count + 2));
    command.push_back(_buffer.add[D::SWAPBLOCK]);
    command.push_back(low_byte(count - 1));
    uint8 checksum = D::WR_DATA + command[1] + command[2] + command[3];
    size_type i;
    for (i = 0; i < count; i++) {
        command.push_back(buff[offset + i]);
        checksum += buff[offset + i];
    }
    command.push_back(checksum);
    for (i = 0; i < 3; i++) {
        if (!check_idle() || (get_port()->write(command.data(), 0, command.size()) != command.size())) {
            get_shared()->last_error = D::SENDDATA_ERROR;
            continue;
        }
        if (send_ok(20 * (command.size() + 6))) {
            return true;
        }
    }
    return false;
}

bool v1_c168_box::commbox_command(uint8 command_word, size_type offset, size_type count, const uint8 *buff) {
    uint8 checksum = low_byte(command_word + count);
    if (command_word < D::WR_DATA) {
        if (count == 0) {
            get_shared()->last_error = D::ILLIGICAL_LEN;
            return false;
        }
        checksum--;
    } else {
        if (count != 0) {
            get_shared()->last_error = D::ILLIGICAL_LEN;
            return false;
        }
    }
    byte_array command;
    command.push_back(checksum + _info.head_password);
    size_type i;
    for (i = 0; i < count; i++) {
        command.push_back(buff[offset + i]);
        checksum += buff[offset + i];
    }
    command.push_back(checksum);

    for (i = 0; i < 3; i++) {
        if (command_word != D::STOP_REC && command_word != D::STOP_EXECUTE) {
            if (!check_idle() || (get_port()->write(command.data(), 0, command.size()) != command.size())) {
                get_shared()->last_error = D::SENDDATA_ERROR;
                continue;
            }
        } else {
            if (get_port()->write(command.data(), 0, command.size()) != command.size()) {
                get_shared()->last_error = D::SENDDATA_ERROR;
                continue;
            }
        }
        if (send_ok(timer::to_ms(20) * command.size())) {
            return true;
        }
    }
    return false;
}

bool v1_c168_box::check_result(int64 microseconds) {
    get_port()->set_read_timeout(microseconds);
    uint8 receive_buffer;
    if (get_port()->read(&receive_buffer, 0, 1) != 1) {
        get_shared()->last_error = D::TIMEOUT_ERROR;
    }
    if (receive_buffer == D::SUCCESS) {
        return true;
    }
    while (get_port()->read(&receive_buffer, 0, 1) == 1);
    get_shared()->last_error = receive_buffer;
    return false;
}

bool v1_c168_box::stop_now(bool is_stop_execute) {
    if (is_stop_execute) {
        uint8 receive_buffer = 0;
        uint32 times = D::REPLAYTIMES;
        while (times--) {
            if (!commbox_do(D::STOP_EXECUTE, 0, 0, NULL)) {
                return false;
            } else {
                if (get_port()->read(&receive_buffer, 0, 1) != 1) {
                    get_shared()->last_error = D::TIMEOUT_ERROR;
                }
                if (receive_buffer == D::RUN_ERR) {
                    return true;
                }
                get_shared()->last_error = D::TIMEOUT_ERROR;
            }
        }
        return false;
    } else {
        return commbox_do(D::STOP_REC, 0, 0, NULL);
    }
}

bool v1_c168_box::do_set(uint8 command_word, size_type offset, size_type count, const uint8 *buff) {
    uint32 times = D::REPLAYTIMES;
    while (times--) {
        if (!commbox_do(command_word, offset, count, buff)) {
            continue;
        } else if (check_result(timer::to_ms(50))) {
            return true;
        }
        stop_now(true);
    }
    return false;
}

size_type v1_c168_box::read_data(uint8 *buff, size_type offset, size_type count, int64 microseconds) {
    get_port()->set_read_timeout(microseconds);
    return get_port()->read(buff, offset, count);
}

size_type v1_c168_box::get_cmd_data(uint8 command, uint8* receive_buffer) {
    uint8 checksum = command;
    uint8 cmd_info[2];
    if (read_data(cmd_info, 0, 2, timer::to_ms(150)) != 2) {
        return 0;
    }
    if ((cmd_info[0]) != command) {
        get_shared()->last_error = cmd_info[0];
        get_port()->discard_in_buffer();
        return 0;
    }
    if ((read_data(receive_buffer, 0, cmd_info[1], timer::to_ms(150)) != (cmd_info[1])) ||
            (read_data(cmd_info, 0, 1, timer::to_ms(150)) != 1)) {
        return 0;
    }
    checksum += cmd_info[1];
    for (size_type i = 0; i < cmd_info[1]; i++) {
        checksum += receive_buffer[i];
    }

    if (checksum != cmd_info[0]) {
        get_shared()->last_error = D::CHECKSUM_ERROR;
        return 0;
    }
    return static_cast<int32> (cmd_info[1]);
}

bool v1_c168_box::check_box() {
    static uint8 password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 0x17, 0x2D, 0x4D};
    srand((unsigned) time(NULL));
    uint8 temp[5];
    temp[4] = 0x00;
    size_type i = 0;
//    while (i < 4) {
//        temp[i] = low_byte(rand());
//        temp[4] += temp[i];
//        i++;
//    }
    temp[0] = 0x5C;
    temp[1] = 0xA1;
    temp[2] = 0x7E;
    temp[3] = 0x01;
    temp[4] = temp[0] + temp[1] + temp[2] + temp[3];
    if (get_port()->write(temp, 0, 5) != 5) {
        get_shared()->last_error = D::SENDDATA_ERROR;
        return false;
    }
    uint8 checksum = temp[4] + temp[4];
    i = 0;
    while (i < sizeof (password)) {
        checksum += password[i] ^ temp[i % 5];
        i++;
    }
    timer::sleep(timer::to_ms(20));
    if (get_cmd_data(D::GETINFO, temp) <= 0) {
        return false;
    }
    _info.head_password = temp[0];
    if (checksum != _info.head_password) {
        get_shared()->last_error = D::CHECKSUM_ERROR;
        return false;
    }
    if (_info.head_password == 0) {
        _info.head_password = 0x55;
    }
    return true;
}

bool v1_c168_box::init_box() {
    get_shared()->is_db_20 = false;

    if (!commbox_do(D::GETINFO, 0, 0, NULL)) {
        return false;
    }

    uint32 length = static_cast<uint32> (get_cmd_data(D::GETINFO, _cmd_temp));
    if (length < D::COMMBOXIDLEN) {
        get_shared()->last_error = D::LOST_VERSIONDATA;
        return false;
    }
    _info.time_unit = 0;
    uint32 pos = 0;
    for (size_type i = 0; i < D::MINITIMELEN; i++) {
        _info.time_unit = _info.time_unit * 256 + _cmd_temp[pos++];
    }

    _info.time_base_db = _cmd_temp[pos++];
    _info.time_extern_db = _cmd_temp[pos++];
    _info.cmd_buff_len = _cmd_temp[pos++];
    if (_info.time_base_db == 0
            || _info.time_unit == 0
            || _info.cmd_buff_len == 0) {
        get_shared()->last_error = D::COMMTIME_ZERO;
        return false;
    }
    for (size_type i = 0; i < D::COMMBOXIDLEN; i++) {
        _info.id[i] = _cmd_temp[pos++];
    }
    for (size_type i = 0; i < D::VERSIONLEN; i++) {
        _info.version[i] = _cmd_temp[pos++];
    }
    _info.port[0] = D::NULLADD;
    _info.port[1] = D::NULLADD;
    _info.port[2] = D::NULLADD;

    _buffer.id = D::NULLADD;
    _buffer.used_num = 0;
    memset(_buffer.add, D::NULLADD, D::MAXIM_BLOCK);
    _buffer.add[D::LINKBLOCK] = _info.cmd_buff_len;
    _buffer.add[D::SWAPBLOCK] = 0;
    return true;
}

bool v1_c168_box::serial_port_change_config() {
    if (_is_initialize) {
        return true;
    }
    get_shared()->last_error = 0;
    uint8 baud = D::UP_57600BPS;
    if (!commbox_do(D::SET_UPBAUD, 0, 1, &baud)) {
        return false;
    }
    timer::sleep(timer::to_ms(50));
    check_result(timer::to_ms(50));
    set_rf(D::SETRFBAUD, D::UP_57600BPS);
    check_result(timer::to_ms(50));
    _serial_port_baud = 57600;
    return true;
}

bool v1_c168_box::check_serial_port_change_config() {
    set_rf(D::SETRFBAUD, D::UP_57600BPS);
    uint8 baud = D::UP_57600BPS;
    if (!commbox_do(D::SET_UPBAUD, 0, 1, &baud)) {
        return false;
    }
    if (!check_result(timer::to_ms(100))) {
        return false;
    }
    get_port()->discard_in_buffer();
    get_port()->discard_out_buffer();
    return true;
}

bool v1_c168_box::open_comm() {
    set_rf(D::RESET_RF, 0);
    set_rf(D::SETDTR_L, 0);
    if (init_box() && check_box()) {
        _is_initialize = false;
        return true;
    }
    return false;

}

bool v1_c168_box::close_comm() {
    stop_now(true);
    do_set(D::RESET, 0, 0, NULL);
    set_rf(D::RESET_RF, 0);
    _is_initialize = true;
    return true;
}

bool v1_c168_box::new_batch(uint8 buff_id) {
    if (buff_id > D::MAXIM_BLOCK) {
        get_shared()->last_error = D::NODEFINE_BUFF;
        return false;
    }
    if (_buffer.id != D::NULLADD) {
        get_shared()->last_error = D::APPLICATION_NOW;
        return false;
    }
    if ((_buffer.add[buff_id] != D::NULLADD && buff_id != D::LINKBLOCK && !del_batch(buff_id))) {
        return false;
    }

    _cmd_temp[0] = D::WR_DATA;
    _cmd_temp[1] = 0x01;
    if (buff_id == D::LINKBLOCK) {
        _cmd_temp[2] = 0xFF;
        _buffer.add[D::LINKBLOCK] = _info.cmd_buff_len;
    } else {
        _cmd_temp[2] = _buffer.add[D::SWAPBLOCK];
    }

    if (_buffer.size() <= 1) {
        get_shared()->last_error = D::BUFFFLOW;
        return false;
    }
    _cmd_temp[3] = D::WR_DATA + 0x01 + _cmd_temp[2];
    _cmd_temp[0] += _info.head_password;
    _buffer.id = buff_id;
    get_shared()->is_do_now = false;
    return true;
}

bool v1_c168_box::add_to_buff(uint8 command_word, size_type offset, size_type count, const uint8 *data) {
    size_type i = 0;
    uint8 checksum = _cmd_temp[_cmd_temp[1] + 2];
    get_shared()->next_address = _cmd_temp[1] + count + 1;

    if (_buffer.id == D::NULLADD) {
        // 数据块标识登记是否有申请?
        get_shared()->last_error = D::NOAPPLICATBUFF;
        get_shared()->is_do_now = true;
        return false;
    }
    if (_buffer.size() < get_shared()->next_address) {
        // 检查是否有足够的空间存储?
        get_shared()->last_error = D::BUFFFLOW;
        get_shared()->is_do_now = true;
        return false;
    }

    if (command_word < D::RESET && command_word != D::CLR_LINK
            && command_word != D::DO_BAT_00 && command_word != D::D0_BAT
            && command_word != D::D0_BAT_FOR && command_word != D::WR_DATA) {
        // 是否为缓冲区命令?
        if ((count <= D::CMD_DATALEN)
                || (command_word == D::SEND_DATA && count < D::SEND_LEN)) {
            // 是否合法命令?
            if (command_word == D::SEND_DATA && get_box_ver() > 0x400) {
                // 增加发送长命令
                _cmd_temp[_cmd_temp[1] + 2] = D::SEND_CMD;
                checksum += D::SEND_CMD;
                _cmd_temp[1]++;
                _cmd_temp[_cmd_temp[1] + 2] = static_cast<uint8> (command_word + count);
                if (count > 0) {
                    _cmd_temp[_cmd_temp[1] + 2]--;
                }
                checksum += _cmd_temp[_cmd_temp[1] + 2];
                _cmd_temp[1]++;
                for (i = 0; i < count; i++, _cmd_temp[1]++) {
                    _cmd_temp[_cmd_temp[1] + 2] = data[i + offset];
                    checksum += data[i + offset];
                }
                _cmd_temp[_cmd_temp[1] + 2] = static_cast<uint8> (checksum + count + 2);
                get_shared()->next_address++;
            } else {
                _cmd_temp[_cmd_temp[1] + 2] = static_cast<uint8> (command_word + count);
                if (count > 0) {
                    _cmd_temp[_cmd_temp[1] + 2]--;
                }
                checksum += _cmd_temp[_cmd_temp[1] + 2];
                _cmd_temp[1]++;
                for (i = 0; i < count; i++, _cmd_temp[1]++) {
                    _cmd_temp[_cmd_temp[1] + 2] = data[i + offset];
                    checksum += data[i + offset];
                }
                _cmd_temp[_cmd_temp[1] + 2] = static_cast<uint8> (checksum + count + 1);
                get_shared()->next_address++; // Ogilvy Xu ad
            }
            return true;
        }
        get_shared()->last_error = D::ILLIGICAL_LEN;
        get_shared()->is_do_now = true;
        return false;
    }
    get_shared()->last_error = D::UNBUFF_CMD;
    get_shared()->is_do_now = true;
    return false;
}

bool v1_c168_box::end_batch() {
    uint32 times = D::REPLAYTIMES;
    get_shared()->is_do_now = true;
    if (_buffer.id == D::NULLADD) {
        // 数据块标识登记是否有申请?
        get_shared()->last_error = D::NOAPPLICATBUFF;
        return false;
    }
    if (_cmd_temp[1] == 0x01) {
        _buffer.id = D::NULLADD;
        get_shared()->last_error = D::NOADDDATA;
        return false;
    }
    while (times--) {
        if (times == 0) {
            _buffer.id = D::NULLADD;
            return false;
        }
        if (!check_idle() || (get_port()->write(_cmd_temp, 0, _cmd_temp[1] + 3) != (_cmd_temp[1] + 3))) {
            continue;
        } else if (send_ok(jm::timer::to_ms(20) * (_cmd_temp[1] + 10))) {
            break;
        }
        if (!stop_now(true)) {
            _buffer.id = D::NULLADD;
            return false;
        }
    }

    if (_buffer.id == D::LINKBLOCK) {
        _buffer.add[D::LINKBLOCK] = static_cast<uint8> (_info.cmd_buff_len - _cmd_temp[1]);
    } else {
        _buffer.add[_buffer.id] = _buffer.add[D::SWAPBLOCK];
        _buffer.used[_buffer.used_num] = _buffer.id;
        _buffer.used_num++;
        _buffer.add[D::SWAPBLOCK] = _buffer.add[D::SWAPBLOCK] + _cmd_temp[1];
    }
    _buffer.id = D::NULLADD;
    return true;
}

bool v1_c168_box::del_batch(uint8 buff_id) {
    if (buff_id > D::MAXIM_BLOCK) {
        // 数据块不存在
        get_shared()->last_error = D::NODEFINE_BUFF;
        return false;
    }
    if (_buffer.id == buff_id) {
        _buffer.id = D::NULLADD;
        return true;
    }
    if (_buffer.add[buff_id] == D::NULLADD) {
        // 数据块标识登记是否有申请?
        get_shared()->last_error = D::NOUSED_BUFF;
        return false;
    }

    if (buff_id == D::LINKBLOCK) {
        _buffer.add[D::LINKBLOCK] = static_cast<uint8> (_info.cmd_buff_len);
    } else {
        size_type i = 0;
        for (; i < _buffer.used_num; i++) {
            if (_buffer.used[i] == buff_id) {
                break;
            }
        }
        uint8 data[3];
        data[0] = _buffer.add[buff_id];
        if (i < _buffer.used_num - 1) {
            data[1] = _buffer.add[_buffer.used[i + 1]];
            data[2] = _buffer.add[D::SWAPBLOCK] - data[1];
            if (!do_set(D::COPY_DATA - D::COPY_DATA % 4, 0, 3, data)) {
                return false;
            }
        } else {
            data[1] = _buffer.add[D::SWAPBLOCK];
        }
        uint8 delete_buff_len = data[1] - data[0];
        for (i = i + 1; i < _buffer.used_num; i++) {
            _buffer.used[i - 1] = _buffer.used[i];
            _buffer.add[_buffer.used[i]] = _buffer.add[_buffer.used[i]] - delete_buff_len;
        }
        _buffer.used_num--;
        _buffer.add[D::SWAPBLOCK] = _buffer.add[D::SWAPBLOCK] - delete_buff_len;
        _buffer.add[buff_id] = D::NULLADD;
    }
    return true;
}

bool v1_c168_box::send_to_box(uint8 command_word, size_type offset, size_type count, const uint8 *buff) {
    if (get_shared()->is_do_now) {
        return do_set(command_word, offset, count, buff);
    } else {
        return add_to_buff(command_word, offset, count, buff);
    }
}

bool v1_c168_box::set_line_level(uint8 value_low, uint8 value_high) {
    // 只有一个字节的数据，设定端口1
    _info.port[1] = _info.port[1] & ~value_low;
    _info.port[1] = _info.port[1] | value_high;
    return send_to_box(D::SETPORT1, 1, 1, _info.port);
}

bool v1_c168_box::set_comm_ctrl(uint8 value_open, uint8 value_close) {
    // 只有一个字节的数据，设定端口2
    _info.port[2] = _info.port[2] & ~value_open;
    _info.port[2] = _info.port[2] | value_close;
    return send_to_box(D::SETPORT2, 2, 1, _info.port);
}

bool v1_c168_box::set_comm_line(uint8 send_line, uint8 recv_line) {
    // 只有一个字节的数据，设定端口0
    if (send_line > 7) {
        send_line = 0x0F;
    }
    if (recv_line > 7) {
        recv_line = 0x0F;
    }
    _info.port[0] = send_line + recv_line * 16;
    return send_to_box(D::SETPORT0, 0, 1, _info.port);
}

bool v1_c168_box::turn_over_one_by_one() {
    // 将原有的接受一个发送一个的标志翻转
    return send_to_box(D::SET_ONEBYONE, 0, 0, NULL);
}

bool v1_c168_box::set_echo_data(uint8* echo_buff, size_type echo_len) {
    if (echo_buff == NULL || echo_len == 0 || echo_len > 4) {
        get_shared()->last_error = D::ILLIGICAL_LEN;
        return false;
    }
    if (get_shared()->is_do_now) {
        uint8 receive_buff[6];
        if (!commbox_do(D::Echo, 0, echo_len, echo_buff)
                || (read_data(receive_buff, 0, echo_len, 100000) != echo_len)) {
            return false;
        }
        for (size_type i = 0; i < echo_len; i++) {
            if (receive_buff[i] != echo_buff[i]) {
                get_shared()->last_error = D::CHECKSUM_ERROR;
                return false;
            }
        }
        return check_result(100000);
    } else {
        return add_to_buff(D::Echo, 0, echo_len, echo_buff);
    }
}

bool v1_c168_box::keep_link(bool is_run_link) {
    if (is_run_link) {
        return send_to_box(D::RUNLINK, 0, 0, NULL);
    } else {
        return send_to_box(D::STOPLINK, 0, 0, NULL);
    }
}

bool v1_c168_box::set_comm_link(uint8 ctrl_word1, uint8 ctrl_word2, uint8 ctrl_word3) {
    uint8 ctrl_word[3]; // 通讯控制字3
    int mode_control = ctrl_word1 & 0xE0;
    ctrl_word[0] = ctrl_word1;
    size_type length = 3;
    if ((ctrl_word1 & 0x04) != 0) {
        get_shared()->is_db_20 = true;
    } else {
        get_shared()->is_db_20 = false;
    }
    if (mode_control == D::SET_VPW || mode_control == D::SET_PWM) {
        return send_to_box(D::SETTING, 0, 1, ctrl_word);
    } else {
        ctrl_word[1] = ctrl_word2;
        ctrl_word[2] = ctrl_word3;
        if (ctrl_word3 == 0) {
            length--;
            if (ctrl_word2 == 0) {
                length--;
            }
        }
        if (mode_control == D::EXRS_232 && length < 2) {
            get_shared()->last_error = D::UNSET_EXRSBIT;
            return false;
        }
        return send_to_box(D::SETTING, 0, length, ctrl_word);
    }
}

bool v1_c168_box::set_comm_baud(double baud) {
    uint8 baud_time[2];
    double instruct_num = ((1000000.0 / (_info.time_unit)) * 1000000) / baud;
    if (get_shared()->is_db_20) {
        instruct_num /= 20;
    }
    instruct_num += 0.5;
    if (instruct_num > 65535 || instruct_num < 10) {
        get_shared()->last_error = D::COMMBAUD_OUT;
        return false;
    }
    baud_time[0] = high_byte(instruct_num);
    baud_time[1] = low_byte(instruct_num);
    if (baud_time[0] == 0) {
        return send_to_box(D::SETBAUD, 1, 1, baud_time);
    } else {
        return send_to_box(D::SETBAUD, 0, 2, baud_time);
    }
}

void v1_c168_box::get_link_time(uint8 type, uint32 time) {
    if (type == D::SETBYTETIME) {
        get_shared()->req_byte_to_byte = time;
    } else if (type == D::SETWAITTIME) {
        get_shared()->req_wait_time = time;
    } else if (type == D::SETRECBBOUT) {
        get_shared()->res_byte_to_byte = time;
    } else if (type == D::SETRECFROUT) {
        get_shared()->res_wait_time = time;
    }
}

bool v1_c168_box::set_comm_time(uint8 type, uint32 time) {
    uint8 time_buff[2];
    get_link_time(type, time);

    if (type == D::SETVPWSTART || type == D::SETVPWRECS) {
        if (D::SETVPWRECS == type) {
            time = (time * 2) / 3;
        }
        type = type + (D::SETBYTETIME & 0xF0);
        time = static_cast<uint32> (time / ((_info.time_unit / 1000000.0)));
    } else {
        time = static_cast<uint32> ((time / _info.time_base_db) / (_info.time_unit / 1000000.0));
    }
    if (time > 65535) {
        get_shared()->last_error = D::COMMTIME_OUT;
        return false;
    }
    if (type == D::SETBYTETIME
            || type == D::SETWAITTIME
            || type == D::SETRECBBOUT
            || type == D::SETRECFROUT
            || type == D::SETLINKTIME) {
        time_buff[0] = high_byte(time);
        time_buff[1] = low_byte(time);
        if (time_buff[0] == 0) {
            return send_to_box(type, 1, 1, time_buff);
        } else {
            return send_to_box(type, 0, 2, time_buff);
        }
    }
    get_shared()->last_error = D::UNDEFINE_CMD;
    return false;
}

bool v1_c168_box::run_receive(uint8 type) {
    if (type == D::GET_PORT1) {
        get_shared()->is_db_20 = false;
    }
    if (type == D::GET_PORT1 || type == D::SET55_BAUD
            || (type >= D::REC_FR && type <= D::RECEIVE)) {
        if (get_shared()->is_do_now) {
            return commbox_do(type, 0, 0, NULL);
        } else {
            return add_to_buff(type, 0, 0, NULL);
        }
    }
    get_shared()->last_error = D::UNDEFINE_CMD;
    return false;
}

uint8 v1_c168_box::get_abs_add(uint8 buff_id, uint8 add) {
    int32 length = 0;
    uint8 start_add = 0;
    if (_buffer.id != buff_id) {
        if (_buffer.add[buff_id] == D::NULLADD) {
            get_shared()->last_error = D::NOUSED_BUFF;
            return 0;
        }
        if (buff_id == D::LINKBLOCK) {
            length = _info.cmd_buff_len - _buffer.add[D::LINKBLOCK];
        } else {
            size_type i;
            for (i = 0; i < _buffer.used_num; i++) {
                if (_buffer.used[i] == buff_id) {
                    break;
                }
            }
            if (i == (_buffer.used_num - 1)) {
                length = _buffer.add[D::SWAPBLOCK] - _buffer.add[buff_id];
            } else {
                length = _buffer.add[buff_id + 1] - _buffer.add[buff_id];
            }
        }
        start_add = _buffer.add[buff_id];
    } else {
        length = _buffer.add[D::LINKBLOCK] - _buffer.add[D::SWAPBLOCK];
        start_add = _buffer.add[D::SWAPBLOCK];
    }
    if (add < length) {
        return add + start_add;
    }
    get_shared()->last_error = D::OUTADDINBUFF;
    return 0;
}

bool v1_c168_box::update_buff(uint8 type, uint8 *buffer) {
    uint8 cmd_temp[4];
    get_shared()->last_error = 0;
    uint8 ret = get_abs_add(buffer[0], buffer[1]);
    if (ret == 0) {
        return false;
    }
    cmd_temp[0] = ret;

    if ((type == D::INVERT_DATA) || (type == D::DEC_DATA) || (type == D::INC_DATA)) {
    } else if ((type == D::UPDATE_1BYTE) || (type == D::SUB_BYTE)) {
        cmd_temp[1] = buffer[2];
    } else if (type == D::INC_2DATA) {
        ret = get_abs_add(buffer[2], buffer[3]);
        if (ret == 0) {
            return false;
        }
        cmd_temp[1] = ret;
    } else if ((type == D::COPY_DATA) || (type == D::ADD_1BYTE)) {
        ret = get_abs_add(buffer[2], buffer[3]);
        if (ret == 0) {
            return false;
        }
        cmd_temp[1] = ret;
        cmd_temp[2] = buffer[4];
    } else if ((type == D::UPDATE_2BYTE) || (type == D::ADD_2BYTE)) {
        ret = get_abs_add(buffer[3], buffer[4]);
        if (ret == 0) {
            return false;
        }
        cmd_temp[1] = buffer[2];
        cmd_temp[2] = ret;
        cmd_temp[3] = buffer[5];

    } else if ((type == D::ADD_DATA) || (type == D::SUB_DATA)) {
        ret = get_abs_add(buffer[2], buffer[3]);
        if (ret == 0) {
            return false;
        }
        cmd_temp[1] = ret;
        ret = get_abs_add(buffer[4], buffer[5]);
        if (ret == 0) {
            return false;
        }
        cmd_temp[2] = ret;

    } else {
        get_shared()->last_error = D::UNDEFINE_CMD;
        return false;

    }
    return send_to_box(type - type % 4, 0, type % 4 + 1, cmd_temp);
}

bool v1_c168_box::commbox_delay(uint32 time) {
    uint8 time_buff[2];
    int delay_word = D::DELAYSHORT;
    time = static_cast<uint32> (time / (_info.time_unit / 1000000.0));
    if (time == 0) {
        get_shared()->last_error = D::SETTIME_ERROR;
        return false;
    }
    if (time > 65535) {
        time = static_cast<uint32> (time / _info.time_base_db);
        if (time > 65535) {
            time = static_cast<uint32> (time / _info.time_base_db);
            if (time > 65535) {
                get_shared()->last_error = D::COMMTIME_OUT;
                return false;
            }
            delay_word = D::DELAYLONG;
        } else {
            delay_word = D::DELAYTIME;
        }
    }
    time_buff[0] = high_byte(time);
    time_buff[1] = low_byte(time);
    if (time_buff[0] == 0) {
        if (get_shared()->is_do_now) {
            return commbox_do(delay_word, 1, 1, time_buff);
        } else {
            return add_to_buff(delay_word, 1, 1, time_buff);
        }
    } else {
        if (get_shared()->is_do_now) {
            return commbox_do(delay_word, 0, 2, time_buff);
        } else {
            return add_to_buff(delay_word, 0, 2, time_buff);
        }
    }
}

bool v1_c168_box::send_out_data(const uint8 *buffer, size_type offset, size_type count) {
    if (count == 0) {
        get_shared()->last_error = D::ILLIGICAL_LEN;
        return false;
    }
    if (get_shared()->is_do_now) {
        return commbox_do(D::SEND_DATA, offset, count, buffer);
    } else {
        return add_to_buff(D::SEND_DATA, offset, count, buffer);
    }
}

bool v1_c168_box::run_batch(uint8 *buff_id, size_type count, bool is_execute_many) {
    int command_word = D::D0_BAT;
    for (size_type i = 0; i < count; i++) {
        if (_buffer.add[buff_id[i]] == D::NULLADD) {
            get_shared()->last_error = D::NOUSED_BUFF;
            return false;
        }
    }
    if (is_execute_many) {
        command_word = D::D0_BAT_FOR;
    }
    if (command_word == D::D0_BAT && buff_id[0] == _buffer.used[0]) {
        command_word = D::DO_BAT_00;
        return commbox_do(command_word, 0, 0, NULL);
    }

    return commbox_do(command_word, 0, count, buff_id);
}

bool v1_c168_box::set_rf(uint8 cmd, uint8 cmd_info) {
    size_type times = D::REPLAYTIMES;
    cmd_info += cmd;
    if (cmd == D::SETRFBAUD) {
        times = 2;
    }
    timer::sleep(timer::to_ms(6));
    while (times--) {
        if (check_idle() && (get_port()->write(&cmd_info, 0, 1) == 1)) {
            if (!send_ok(timer::to_ms(50))) {
                continue;
            }
            if ((get_port()->write(&cmd_info, 0, 1) != 1) ||
                    !check_result(timer::to_ms(150))) {
                continue;
            }
            timer::sleep(timer::to_ms(100));
            return true;
        }
    }
    return false;
}

size_type v1_c168_box::read_bytes(uint8 *buff, size_type offset, size_type count) {
    return read_data(buff, offset, count, get_shared()->res_wait_time);
}

int32 v1_c168_box::serial_port_baud() {
    return _serial_port_baud;
}

uint8 v1_c168_box::serial_port_databits() {
    return _serial_port_databits;
}

int32 v1_c168_box::serial_port_parity() {
    return _serial_port_parity;
}

int32 v1_c168_box::serial_port_stopbits() {
    return _serial_port_stopbits;
}

int32 v1_c168_box::serial_port_flow_control() {
    return _serial_port_flow_control;
}

}
}
