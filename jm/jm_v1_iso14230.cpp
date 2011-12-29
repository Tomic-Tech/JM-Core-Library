#include "jm_v1_iso14230.hpp"
#include "jm_timer.hpp"

namespace jm {
namespace v1 {

v1_iso14230::v1_iso14230(const v1_box_ptr &box, const v1_shared_ptr &shared)
: _box(box)
, _shared(shared)
, _default(boost::make_shared<v1_default<v1_iso14230> >(_box, _shared, this))
, _l_line(false)
, _send_line(0)
, _recv_line(0) {
}

size_t v1_iso14230::read_one_frame(const uint8 *data, size_t offset, bool is_finish) {
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
            result.push_back(temp, 3);
            result.push_back(b);
            result.resize(KWP80HeaderLength + length + KWPChecksumLength);
            length = _box->read_bytes(result.data(), KWP80HeaderLength, length + KWPChecksumLength);
        } else {
            length = temp[0] - 0x80;
            if (length <= 0) {
                return 0;
            }
            result.push_back(temp, 3);
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
            result.push_back(temp, 3);
            result.resize(KWP00HeaderLength + length + KWPChecksumLength);
            length = _box->read_bytes(result.data(), 3, length);
        } else {
            length = temp[0];
            if (length <= 0) {
                finish_execute(is_finish);
                return 0;
            }
            result.push_back(temp, 3);
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

error_code v1_iso14230::fast_init(const uint8 *data, size_t offset, size_t count) {
    uint8 value_open = 0;
    if (_l_line) {
        value_open = _box->pwc() | _box->rzfc() | _box->ck();
    } else {
        value_open = _box->pwc() | _box->rzfc() | _box->ck();
    }
    if (!_box->set_comm_ctrl(value_open, _box->set_null())
            || !_box->set_comm_line(_send_line, _recv_line)
            || !_box->set_comm_link(_box->rs_232() | _box()->bit9_mark() | _box()->sel_sl() | _box()->un_db20(), _box > set_null(), _box->set_null())
            || !_box->set_comm_baud(10416)
            || !_box->set_comm_time(_box->set_byte_time(), timer::to_ms(5))
            || !_box->set_comm_time(_box->set_wait_time(), timer::to_ms(0))
            || !_box->set_comm_time(_box->set_rec_bb_out(), timer::to_ms(400))
            || !_box->set_comm_time(_box->set_rec_fr_out(), timer::to_ms(500))
            || !_box->set_comm_time(_box->set_link_time(), timer::to_ms(500))) {
        return error::generic_error;
    }
    
    timer::sleep(timer::to_sec(1));
    _shared->buff_id = 0;
    if (!_box->new_batch(_shared->buff_id)) {
        return error::generic_error;
    }
    byte_array pack_enter;
    pack(byte_array(data, offset, count), pack_enter);
    if (!_box->set_line_level(_box->coms(), _box->set_null())
            || !_box->commbox_delay(timer::to_ms(25))
            || !_box->set_line_level(_box->set_null(), _box->coms())
            || !_box->commbox_delay(timer::to_ms(25))
            || !_box->send_out_data(pack_enter.data(), 0, pack_enter.size())
            || !_box->run_receive(_box->rec_fr())
            || !_box->end_batch()) {
        _box->del_batch(_shared->buff_id);
        return error::generic_error;
    }
    if (!_box->run_batch(&_shared->buff_id, 1, false))
        return error::generic_error;
    
    if (read_one_frame(pack_enter) <= 0)
        return error::generic_error;
    _box->set_comm_time(_box->set_wait_time(), timer::to_ms(55));
}

}
}
