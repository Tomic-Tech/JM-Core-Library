#include "jm_protocol.hpp"

namespace jm {

size_type protocol::pack(const byte_array &src, byte_array &tar) {
    uint8 buff[256];
    tar.clear();
    size_type ret = pack(src.data(), 0, src.size(), buff, 0);
    if (ret > 0) {
        tar.push_back(buff, 0, ret);
    }
    return ret;
}

size_type protocol::unpack(const byte_array &src, byte_array &tar) {
    uint8 buff[256];
    tar.clear();
    size_type ret = unpack(src.data(), 0, src.size(), buff, 0);
    if (ret > 0) {
        tar.push_back(buff, 0, ret);
    }
    return ret;
}

size_type protocol::send_one_frame(const byte_array &data) {
    return send_one_frame(data.data(), 0, data.size());
}

size_type protocol::read_one_frame(byte_array &data) {
    uint8 buff[256];
    size_type ret = read_one_frame(buff, 0);
    data.clear();
    if (ret > 0) {
        data.push_back(buff, 0, ret);
    }
    return ret;
}

}
