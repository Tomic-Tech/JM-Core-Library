#include "jm_mikuni.hpp"

namespace jm {

size_t mikuni::pack(const uint8 *src, size_t src_offset, size_t count,
        uint8 *tar, size_t tar_offset) {
    if (count <= 0)
        return 0;

    size_t pos = tar_offset;
    tar[pos++] = HEAD_FORMAT;
    memcpy(tar + pos, src + src_offset, count);
    pos += count;
    tar[pos++] = 0x0D;
    tar[pos++] = 0x0A;
    return pos - tar_offset;
}

size_t mikuni::unpack(const uint8 *src, size_t src_offset, size_t count,
        uint8 *tar, size_t tar_offset) {
    if (count <= 0)
        return 0;

    memcpy(tar + tar_offset, src + src_offset + 1, count - 2);
    return count - 2;
}

}
