#include "jm_canbus.hpp"
#include "jm_utils.hpp"

namespace jm {

canbus::canbus() {
    _target_id = 0;
    _baud = B500K;
    _id_mode = STD;
    _filter_mask = Disable;
    _frame_type = Data;
    _high = 0;
    _low = 0;
    _flow_control[0] = 0x30;
    _flow_control[1] = 0x00;
    _flow_control[2] = 0x00;
    _flow_control[3] = 0x00;
    _flow_control[4] = 0x00;
    _flow_control[5] = 0x00;
    _flow_control[6] = 0x00;
    _flow_control[7] = 0x00;
}

size_type canbus::pack(const uint8 *src, size_type src_offset, size_type count,
        uint8 *tar, size_type tar_offset) {

    if (count > 8 || count <= 0) {
        return 0;
    }

    if (_id_mode == STD) {
        tar[tar_offset + 1] = high_byte(low_word(_target_id));
        tar[tar_offset + 2] = low_byte(low_word(_target_id));
        if (_frame_type == Data) {
            tar[tar_offset] = low_byte(count | STD | Data);
        } else {
            tar[tar_offset] = low_byte(count | STD | Remote);
        }
        memcpy(tar + tar_offset + 3, src + src_offset, count);
        return count + 3;
    } else if (_id_mode == EXT) {
        tar[tar_offset + 1] = high_byte(high_word(_target_id));
        tar[tar_offset + 2] = low_byte(high_word(_target_id));
        tar[tar_offset + 3] = high_byte(low_word(_target_id));
        tar[tar_offset + 4] = low_byte(low_word(_target_id));
        if (_frame_type == Data) {
            tar[tar_offset] = low_byte(count | EXT | Data);
        } else {
            tar[tar_offset] = low_byte(count | EXT | Remote);
        }
        memcpy(tar + tar_offset + 5, src + src_offset, count);
        return count + 5;
    }
    return 0;
}

size_type canbus::unpack(const uint8 *src, size_type src_offset, size_type count,
        uint8 *tar, size_type tar_offset) {
    size_type length = 0;
    if (count <= 0) {
        return 0;
    }

    int32 mode = (src[src_offset] & (EXT | Remote));
    if (mode == (STD | Data)) {
        length = src[src_offset] & 0x0F;
        if (length != count - 3) {
            return 0;
        }
        memcpy(tar + tar_offset, src + src_offset + 3, length);
    } else if (mode == (STD | Remote)) {
    } else if (mode == (EXT | Data)) {
        length = src[0] & 0x0F;
        if (length != count - 5) {
            return 0;
        }
        memcpy(tar + tar_offset, src + src_offset + 5, length);
    } else {
    }
    return length;
}


}
