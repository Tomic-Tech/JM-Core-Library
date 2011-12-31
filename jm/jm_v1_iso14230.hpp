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
    , _default(boost::make_shared<v1_default<BOX, v1_iso14230> >(_box, _shared, this))
    , _l_line(false)
    , _send_line(0)
    , _recv_line(0) {
    }

    error_code fast_init(const uint8 *data, size_t offset, size_t count) {
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
                || !_box->set_comm_time(BOX::D::SETBYTETIME, jm::timer::to_ms(5))
                || !_box->set_comm_time(BOX::D::SETWAITTIME, jm::timer::to_ms(0))
                || !_box->set_comm_time(BOX::D::SETRECBBOUT, jm::timer::to_ms(400))
                || !_box->set_comm_time(BOX::D::SETRECFROUT, jm::timer::to_ms(500))
                || !_box->set_comm_time(BOX::D::SETLINKTIME, jm::timer::to_ms(500))) {
            return error::generic_error;
        }

        jm::timer::sleep(jm::timer::to_sec(1));
        _shared->buff_id = 0;
        if (!_box->new_batch(_shared->buff_id)) {
            return error::generic_error;
        }
        uint8 pack_enter[256];
        size_t length = pack(data, offset, count, pack_enter, 0);
        if (!_box->set_line_level(BOX::D::COMS, BOX::D::SET_NULL)
                || !_box->commbox_delay(jm::timer::to_ms(25))
                || !_box->set_line_level(BOX::D::SET_NULL, BOX::D::COMS)
                || !_box->commbox_delay(jm::timer::to_ms(25))
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
        _box->set_comm_time(BOX::D::SETWAITTIME, jm::timer::to_ms(55));
    }

    error_code addr_init(uint8 addr_code) {
        if (!_box->set_comm_ctrl(BOX::D::PWC | BOX::D::REFC | BOX::D::RZFC | BOX::D::CK, BOX::D::SET_NULL)
                || !_box->set_comm_link(BOX::D::RS_232 | BOX::D::BIT9_MARK | BOX::D::SEL_SL | BOX::D::SET_DB20, BOX::D::SET_NULL, BOX::D::INVERTBYTE)
                || !_box->set_comm_baud(5)
                || !_box->set_comm_time(BOX::D::SETBYTETIME, jm::timer::to_ms(5))
                || !_box->set_comm_time(BOX::D::SETWAITTIME, jm::timer::to_ms(12))
                || !_box->set_comm_time(BOX::D::SETRECBBOUT, jm::timer::to_ms(400))
                || !_box->set_comm_time(BOX::D::SETRECFROUT, jm::timer::to_ms(500))
                || !_box->set_comm_time(BOX::D::SETLINKTIME, jm::timer::to_ms(500))) {
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
                || !_box->set_comm_time(BOX::D::SETWAITTIME, jm::timer::to_ms(55))) {
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
    }

    size_t send_one_frame(const uint8 *data, size_t offset, size_t count) {
        return _default->send_one_frame(data, offset, count, false);
    }

    size_t send_frames(const uint8 *data, size_t offset, size_t count) {
        return send_one_frame(data, offset, count);
    }

    size_t read_one_frame(uint8 *data, size_t offset) {
        return read_one_frame(data, offset, true);
    }

    size_t read_frames(uint8 *data, size_t offset) {
        return read_one_frame(data, offset);
    }

    void finish_execute(bool is_finish) {
        if (is_finish) {
            _box->stop_now(true);
            _box->del_batch(_shared->buff_id);
            _box->check_result(jm::timer::to_ms(500));
        }
    }
    error_code set_keep_link(const uint8 *data, size_t offset, size_t count) {
        _mode = _link_mode;
        uint8 buff[256];
        size_t length = pack(data, offset, count, buff, 0);
        _mode = _msg_mode;
        return _default->set_keep_link(buff, 0, length);
    }
private:

    size_t read_one_frame(uint8 *data, size_t offset, bool is_finish) {
        uint8 temp[3];
        int32 length = _box->read_bytes(temp, 0, 3);
        if (length <= 0) {
            finish_execute(is_finish);
            return 0;
        }

        byte_array result;
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
                result.push_back(temp, 0, 3);
                result.push_back(b);
                result.resize(KWP80HeaderLength + length + KWPChecksumLength);
                length = _box->read_bytes(result.data(), KWP80HeaderLength, length + KWPChecksumLength);
            } else {
                length = temp[0] - 0x80;
                if (length <= 0) {
                    return 0;
                }
                result.push_back(temp, 0, 3);
                result.resize(KWP8XHeaderLength + length + KWPChecksumLength);
                length = _box->read_bytes(result.data(), 3, length + KWPChecksumLength);
            }
        } else {
            if (temp[0] == 0x00) {
                length = temp[1];
                if (length <= 0) {
                    finish_execute(is_finish);
                    return 0;
                }
                result.push_back(temp, 0, 3);
                result.resize(KWP00HeaderLength + length + KWPChecksumLength);
                length = _box->read_bytes(result.data(), 3, length);
            } else {
                length = temp[0];
                if (length <= 0) {
                    finish_execute(is_finish);
                    return 0;
                }
                result.push_back(temp, 0, 3);
                result.resize(KWPXXHeaderLength + length + KWPChecksumLength);
                length = _box->read_bytes(result.data(), 3, length - KWPChecksumLength);
            }
        }

        finish_execute(is_finish);
        if (length <= 0)
            return length;

        uint8 checksum = 0;
        for (std::size_t i = 0; i < result.size() - 1; i++) {
            checksum += data[i];
        }

        if (checksum != result.back())
            return 0;

        return unpack(result.data(), 0, result.size(), data, offset);
    }

};

}
}

#endif	/* JM_V1_ISO14230_HPP */

