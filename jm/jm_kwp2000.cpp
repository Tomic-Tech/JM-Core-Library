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

int32 kwp2000::pack(const byte_array &source, byte_array &target) {
    if (source.size() <= 0) {
        return -1;
    }
    
    target.clear();
    if (_mode == KWP8X) {
        target.push_back(low_byte(0x80 | source.size()));
        target.push_back(low_byte(_target_address));
        target.push_back(low_byte(_source_address));
        target.push_back(source);
    } else if (_mode == KWPCX) {
        target.push_back(low_byte(0xC0 | source.size()));
        target.push_back(low_byte(_target_address));
        target.push_back(low_byte(_source_address));
        target.push_back(source);
    } else if (_mode == KWP80) {
        target.push_back(low_byte(0x80));
        target.push_back(low_byte(_target_address));
        target.push_back(low_byte(_source_address));
        target.push_back(low_byte(source.size()));
        target.push_back(source);
    } else if (_mode == KWP00) {
        target.push_back(0x00);
        target.push_back(low_byte(source.size()));
        target.push_back(source);
    } else {
        target.push_back(low_byte(source.size()));
        target.push_back(source);
    }
    
    uint8 checksum = 0;
    for (std::size_t i = 0; i < target.size(); i++) {
        checksum += target[i];
    }
    target.push_back(checksum);
    return target.size();
}

int32 kwp2000::unpack(const byte_array &source, byte_array &target) {
    if (source.size() <= 0) {
        return -1;
    }
    target.clear();
    int32 length = 0;
    if (source[0] > 0x80) {
        length = source[0] - 0x80;
        if (source[1] != _source_address) {
            return -1;
        }
        if (length != (source.size() - KWP8XHeaderLength - KWPChecksumLength)) {
            length = source[0] - 0xC0; // for KWPCX
            if (length != (source.size() - KWPCXHeaderLength - KWPChecksumLength)) {
                return -1;
            }
        }
        target.push_back(source.data() + KWP8XHeaderLength, length);
    } else if (source[0] == 0x80) {
        length = source[3];
        if (source[1] != _source_address) {
            return -1;
        }
        if (length != (source.size() - KWP80HeaderLength - KWPChecksumLength)) {
            return -1;
        }
        target.push_back(source.data() + KWP80HeaderLength, length);
    } else if (source[0] == 0) {
        length = source[1];
        if (length != (source.size() - KWP00HeaderLength - KWPChecksumLength)) {
            return -1;
        }
        target.push_back(source.data() + KWP00HeaderLength, length);
    } else {
        length = source[0];
        if (length != (source.size() - KWP00HeaderLength - KWPChecksumLength)) {
            return -1;
        }
        target.push_back(source.data() + KWP00HeaderLength, length);
    }
    return target.size();
}

}
