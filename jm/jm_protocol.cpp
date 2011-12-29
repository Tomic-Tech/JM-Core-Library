#include "jm_protocol.hpp"

namespace jm {

size_t protocol::pack(const byte_array &src, byte_array &tar) {
    uint8 buff[256];
    tar.clear();
    size_t ret = pack(src.data(), 0, src.size(), buff, 0);
    if (ret > 0) {
        tar.push_back(buff, 0, ret);
    }
    return ret;
}

size_t protocol::unpack(const byte_array &src, byte_array &tar) {
    uint8 buff[256];
    tar.clear();
    size_t ret = unpack(src.data(), 0, src.size(), buff, 0);
    if (ret > 0) {
        tar.push_back(buff, 0, ret);
    }
    return ret;
}

}
