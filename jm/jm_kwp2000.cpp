#include "jm_kwp2000.hpp"
#include "jm_utils.hpp"

namespace jm {

kwp2000::kwp2000() : _mode(KWP8X), _msg_mode(KWP8X), _link_mode(KWP8X), _baud(0) {

}

void kwp2000::set_options(kwp_mode msg_mode, kwp_mode link_mode, int32 baud) {
    _msg_mode = msg_mode;
    _link_mode = link_mode;
    _mode = _msg_mode;
    _baud = baud;
}

size_t kwp2000::pack(const uint8 *src, size_t src_offset, size_t count,
        uint8 *tar, size_t tar_offset) {
    if (count <= 0)
        return 0;

    size_t pos = src_offset;

    if (_mode == KWP8X) {
        tar[pos++] = low_byte(0x80 | count);
        tar[pos++] = low_byte(_target_address);
        tar[pos++] = low_byte(_source_address);
        memcpy(tar + pos, src + src_offset, count);
        pos += count;
    } else if (_mode == KWPCX) {
        tar[pos++] = low_byte(0xC0 | count);
        tar[pos++] = low_byte(_target_address);
        tar[pos++] = low_byte(_source_address);
        memcpy(tar + pos, src + src_offset, count);
        pos += count;
    } else if (_mode == KWP80) {
        tar[pos++] = low_byte(0x80);
        tar[pos++] = low_byte(_target_address);
        tar[pos++] = low_byte(_source_address);
        tar[pos++] = low_byte(count);
        memcpy(tar + pos, src + src_offset, count);
        pos += count;
    } else if (_mode == KWP00) {
        tar[pos++] = 0x00;
        tar[pos++] = low_byte(count);
        memcpy(tar + pos, src + src_offset, count);
        pos += count;
    } else {
        tar[pos++] = low_byte(count);
        memcpy(tar + pos, src + src_offset, count);
        pos += count;
    }

    uint8 checksum = 0;
    for (std::size_t i = tar_offset; i < pos; i++) {
        checksum += tar[i];
    }
    tar[pos++] = checksum;
    return pos - tar_offset;
}

size_t kwp2000::unpack(const uint8 *src, size_t src_offset, size_t count,
        uint8 *tar, size_t tar_offset) {
    if (count <= 0)
        return 0;
    
    size_t length = 0;
    if (src[0] > 0x80) {
        length = src[0] - 0x80;
        if (src[1] != _source_address) {
            return 0;
        }
        if (length != (count - KWP8XHeaderLength - KWPChecksumLength)) {
            length = src[0] - 0xC0; // for KWPCX
            if (length != (count - KWPCXHeaderLength - KWPChecksumLength)) {
                return 0;
            }
        }
        memcpy(tar + tar_offset, src + src_offset + KWP8XHeaderLength, length);
    } else if (src[0] == 0x80) {
        length = src[3];
        if (src[1] != _source_address) {
            return 0;
        }
        if (length != (count - KWP80HeaderLength - KWPChecksumLength)) {
            return 0;
        }
        memcpy(tar + tar_offset, src + src_offset + KWP80HeaderLength, length);
    } else if (src[0] == 0) {
        length = src[1];
        if (length != (count - KWP00HeaderLength - KWPChecksumLength)) {
            return 0;
        }
        memcpy(tar + tar_offset, src + src_offset + KWP00HeaderLength, length);
    } else {
        length = src[0];
        if (length != (count - KWP00HeaderLength - KWPChecksumLength)) {
            return 0;
        }
        memcpy(tar + tar_offset, src + src_offset + KWP00HeaderLength, length);
    }
    return length;
}

error_code kwp2000::fast_init(const byte_array &data) {
    return fast_init(data.data(), 0, data.size());
}

}
