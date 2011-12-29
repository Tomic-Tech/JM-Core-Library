#include "jm_kwp1281.hpp"
#include "jm_utils.hpp"

namespace jm {

kwp1281::kwp1281() {
    _frame_counter = 0;
}

size_t kwp1281::pack(const uint8 *src, size_t src_offset, size_t count,
        uint8 *tar, size_t tar_offset) {

    if (count <= 0) {
        return 0;
    }

    size_t pos = tar_offset;
    tar[pos++] = low_byte(count + 20);
    tar[pos++] = frame_counter_increment();
    memcpy(tar + pos, src + src_offset, count);
    pos += count;
    tar[pos++] = FRAME_END;
    return pos - tar_offset;
}

size_t kwp1281::unpack(const uint8 *src, size_t src_offset, size_t count,
        uint8 *tar, size_t tar_offset) {
    if ((count - 2) <= 0) {
        return 0;
    }

    memcpy(tar + tar_offset, src + src_offset + 1, count - 2);
    return count - 2;
}

uint8 kwp1281::frame_counter_increment() {
    return ++_frame_counter;
}

}
