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

int32 canbus::pack(const byte_array &source, byte_array &target) {
    target.clear();
    if (source.size() > 8 || source.size() <= 0) {
        return -1;
    }
    
    if (_id_mode == STD) {
        target.push_back(0);
        target.push_back(high_byte(low_word(_target_id)));
        target.push_back(low_byte(low_word(_target_id)));
        if (_frame_type == Data) {
            target[0] = low_byte(source.size() | STD | Data);
        } else {
            target[0] = low_byte(source.size() | STD | Remote);
        }
        target.push_back(target);
        return target.size();
    } else if (_id_mode == EXT) {
        target.push_back(0);
        target.push_back(high_byte(high_word(_target_id)));
        target.push_back(low_byte(high_word(_target_id)));
        target.push_back(high_byte(low_word(_target_id)));
        target.push_back(low_byte(low_word(_target_id)));
        if (_frame_type == Data) {
            target[0] = low_byte(source.size() | EXT | Data);
        } else {
            target[0] = low_byte(source.size() | EXT | Remote);
        }
        target.push_back(source);
        return target.size();
    }
    return -1;
}

int32 canbus::unpack(const byte_array &source, byte_array &target) {
    target.clear();
    int32 length = 0;
    if (source.size() <= 0) {
        return -1;
    }
    
    int32 mode = (source[0] & (EXT | Remote));
    if (mode == (STD | Data)) {
        length = source[0] & 0x0F;
        if (length != source.size() - 3) {
            return -1;
        }
        target.push_back(source.data() + 3, length);
    } else if (mode == (STD | Remote)) {
    } else if (mode == (EXT | Data)) {
        length = source[0] & 0x0F;
        if (length != source.size() - 5) {
            return -1;
        }
        target.push_back(source.data() + 5, length);
    } else {
    }
    return target.size();
}


}
