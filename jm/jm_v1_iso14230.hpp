/* 
 * File:   jm_v1_iso14230.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月25日, 下午11:00
 */

#ifndef JM_V1_ISO14230_HPP
#define	JM_V1_ISO14230_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_kwp2000.hpp>
#include <jm_v1_box.hpp>
#include <jm_timer.hpp>
#include <jm_v1_default.hpp>

namespace jm {
namespace v1 {

template <typename BOX>
class v1_iso14230 : public kwp2000 {
private:
    boost::shared_ptr<BOX> _box;
    v1_shared_ptr _shared;
    boost::shared_ptr<v1_default<BOX, v1_iso14230> > _default;
    bool _l_line;
    int32 _send_line;
    int32 _recv_line;

public:

    v1_iso14230(const boost::shared_ptr<BOX> &box, const v1_shared_ptr &shared)
    : _box(box)
    , _shared(shared)
    , _l_line(false)
    , _send_line(0)
    , _recv_line(0) {
        _default = boost::make_shared<v1_default<BOX, v1_iso14230> >(_box, _shared, this);
    }

    error_code fast_init(const uint8 *data, size_type offset, size_type count) {
        uint8 value_open = 0;
        if (_l_line) {
            value_open = BOX::D::PWC | BOX::D::RZFC | BOX::D::CK;
        } else {
            value_open = BOX::D::PWC | BOX::D::RZFC | BOX::D::CK;
        }
        if (!_box->set_comm_ctrl(value_open, BOX::D::SET_NULL)
                || !_box->set_comm_line(_send_line, _recv_line)
                || !_box->set_comm_link(BOX::D::RS_232 | BOX::D::BIT9_MARK | BOX::D::SEL_SL | BOX::D::UN_DB20, BOX::D::SET_NULL, BOX::D::SET_NULL)
                || !_box->set_comm_baud(10416)
                || !_box->set_comm_time(BOX::D::SETBYTETIME, static_cast<uint32> (jm::timer::to_ms(5)))
                || !_box->set_comm_time(BOX::D::SETWAITTIME, static_cast<uint32> (jm::timer::to_ms(0)))
                || !_box->set_comm_time(BOX::D::SETRECBBOUT, static_cast<uint32> (jm::timer::to_ms(400)))
                || !_box->set_comm_time(BOX::D::SETRECFROUT, static_cast<uint32> (jm::timer::to_ms(500)))
                || !_box->set_comm_time(BOX::D::SETLINKTIME, static_cast<uint32> (jm::timer::to_ms(500)))) {
            return error::generic_error;
        }

        jm::timer::sleep(jm::timer::to_sec(1));
        _shared->buff_id = 0;
        if (!_box->new_batch(_shared->buff_id)) {
            return error::generic_error;
        }
        uint8 pack_enter[256];
        size_type length = pack(data, offset, count, pack_enter, 0);
        if (!_box->set_line_level(BOX::D::COMS, BOX::D::SET_NULL)
                || !_box->commbox_delay(static_cast<uint32> (jm::timer::to_ms(25)))
                || !_box->set_line_level(BOX::D::SET_NULL, BOX::D::COMS)
                || !_box->commbox_delay(static_cast<uint32> (jm::timer::to_ms(25)))
                || !_box->send_out_data(pack_enter, 0, length)
                || !_box->run_receive(BOX::D::REC_FR)
                || !_box->end_batch()) {
            _box->del_batch(_shared->buff_id);
            return error::generic_error;
        }
        if (!_box->run_batch(&_shared->buff_id, 1, false))
            return error::generic_error;

        length = read_one_frame(pack_enter, 0);
        if (length <= 0)
            return error::generic_error;
        _box->set_comm_time(BOX::D::SETWAITTIME, static_cast<uint32> (jm::timer::to_ms(55)));
        return error::success;
    }

    error_code addr_init(uint8 addr_code) {
        if (!_box->set_comm_ctrl(BOX::D::PWC | BOX::D::REFC | BOX::D::RZFC | BOX::D::CK, BOX::D::SET_NULL)
                || !_box->set_comm_link(BOX::D::RS_232 | BOX::D::BIT9_MARK | BOX::D::SEL_SL | BOX::D::SET_DB20, BOX::D::SET_NULL, BOX::D::INVERTBYTE)
                || !_box->set_comm_baud(5)
                || !_box->set_comm_time(BOX::D::SETBYTETIME, static_cast<uint32> (jm::timer::to_ms(5)))
                || !_box->set_comm_time(BOX::D::SETWAITTIME, static_cast<uint32> (jm::timer::to_ms(12)))
                || !_box->set_comm_time(BOX::D::SETRECBBOUT, static_cast<uint32> (jm::timer::to_ms(400)))
                || !_box->set_comm_time(BOX::D::SETRECFROUT, static_cast<uint32> (jm::timer::to_ms(500)))
                || !_box->set_comm_time(BOX::D::SETLINKTIME, static_cast<uint32> (jm::timer::to_ms(500)))) {
            return error::generic_error;
        }
        jm::timer::sleep(jm::timer::to_sec(1));

        _shared->buff_id = 0;
        if (!_box->new_batch(_shared->buff_id))
            return error::generic_error;

        if (!_box->send_out_data(&addr_code, 0, 1)
                || !_box->set_comm_line((_recv_line == BOX::D::RK_NO) ? _send_line : BOX::D::SK_NO, _recv_line)
                || !_box->run_receive(BOX::D::SET55_BAUD)
                || !_box->run_receive(BOX::D::REC_LEN_1)
                || !_box->turn_over_one_by_one()
                || !_box->run_receive(BOX::D::REC_LEN_1)
                || !_box->turn_over_one_by_one()
                || !_box->run_receive(BOX::D::REC_LEN_1)
                || !_box->end_batch()) {
            _box->del_batch(_shared->buff_id);
            return error::generic_error;
        }

        uint8 temp[3];
        if (!_box->run_batch(&_shared->buff_id, 1, false)
                || (_box->read_data(temp, 0, 3, jm::timer::to_sec(3)) <= 0)
                || !_box->check_result(jm::timer::to_sec(5))) {
            _box->del_batch(_shared->buff_id);
            return error::generic_error;
        }
        if (!_box->del_batch(_shared->buff_id)
                || !_box->set_comm_time(BOX::D::SETWAITTIME, static_cast<uint32> (jm::timer::to_ms(55)))) {
            return error::generic_error;
        }

        if (temp[2] != 0)
            return error::generic_error;
        return error::success;
    }

    error_code set_lines(int32 com_line, bool l_line) {
        // According the connector to determine the send and receive line
        if (_shared->connector == OBDII_16) {
            switch (com_line) {
            case 7:
                _send_line = BOX::D::SK1;
                _recv_line = BOX::D::RK1;
                break;
            default:
                return error::generic_error;
            }
        } else {
            return error::generic_error;
        }
        _l_line = l_line;
        return error::success;
    }

    size_type send_one_frame(const uint8 *data, size_type offset, size_type count) {
        return _default->send_one_frame(data, offset, count, false);
    }

    size_type send_frames(const uint8 *data, size_type offset, size_type count) {
        return send_one_frame(data, offset, count);
    }

    size_type read_one_frame(uint8 *data, size_type offset) {
        return read_one_frame(data, offset, true);
    }

    size_type read_frames(uint8 *data, size_type offset) {
        return read_one_frame(data, offset);
    }

    void finish_execute(bool is_finish) {
        if (is_finish) {
            _box->stop_now(true);
            _box->del_batch(_shared->buff_id);
            _box->check_result(jm::timer::to_ms(500));
        }
    }

    error_code set_keep_link(const uint8 *data, size_type offset, size_type count) {
        _mode = _link_mode;
        uint8 buff[256];
        size_type length = pack(data, offset, count, buff, 0);
        _mode = _msg_mode;
        return _default->set_keep_link(buff, 0, length);
    }
private:

    size_type read_one_frame(uint8 *data, size_type offset, bool is_finish) {
        uint8 temp[3];
        size_type frame_length = 0;
        size_type length = _box->read_bytes(temp, 0, 3);
        if (length <= 0) {
            finish_execute(is_finish);
            return 0;
        }

        uint8 result[256];
        if (temp[1] == _source_address) {
            if (temp[0] == 0x80) {
                uint8 b;
                length = _box->read_bytes(&b, 0, 1);
                if (length <= 0) {
                    finish_execute(is_finish);
                    return 0;
                }
                length = b;
                if (length <= 0) {
                    finish_execute(is_finish);
                    return 0;
                }
                memcpy(result, temp, 3);
                frame_length += 3;
                memcpy(result + frame_length, &b, 1);
                frame_length++;
                length = _box->read_bytes(result, KWP80HeaderLength, length + KWPChecksumLength);
                frame_length += length;
            } else {
                length = temp[0] - 0x80;
                if (length <= 0) {
                    return 0;
                }
                memcpy(result, temp, 3);
                frame_length += 3;
                length = _box->read_bytes(result, 3, length + KWPChecksumLength);
                frame_length += length;
            }
        } else {
            if (temp[0] == 0x00) {
                length = temp[1];
                if (length <= 0) {
                    finish_execute(is_finish);
                    return 0;
                }
                memcpy(result, temp, 3);
                frame_length += 3;
                length = _box->read_bytes(result, 3, length);
                frame_length += length;
            } else {
                length = temp[0];
                if (length <= 0) {
                    finish_execute(is_finish);
                    return 0;
                }
                memcpy(result, temp, 3);
                frame_length += 3;
                length = _box->read_bytes(result, 3, length - KWPChecksumLength);
                frame_length += length;
            }
        }

        finish_execute(is_finish);
        if (frame_length <= 0)
            return frame_length;

        uint8 checksum = 0;
        for (size_type i = 0; i < frame_length - 1; i++) {
            checksum += result[i];
        }

        if (checksum != result[frame_length - 1])
            return 0;

        return unpack(result, 0, frame_length, data, offset);
    }

};

}
}

#endif	/* JM_V1_ISO14230_HPP */

