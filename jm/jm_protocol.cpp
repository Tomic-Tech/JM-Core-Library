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

size_t protocol::send_one_frame(const byte_array &data) {
    return send_one_frame(data.data(), 0, data.size());
}

size_t protocol::read_one_frame(byte_array &data) {
    uint8 buff[256];
    size_t ret = read_one_frame(buff, 0);
    data.clear();
    if (ret > 0) {
        data.push_back(buff, 0, ret);
    }
    return ret;
}

}
