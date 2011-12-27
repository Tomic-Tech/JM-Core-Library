#include "jm_mikuni.hpp"

namespace jm {

int32 Mikuni::pack(const byte_array &source, byte_array &target) {
    if (source.size() <= 0) {
        return -1;
    }
    
    target.clear();
    target.push_back(HEAD_FORMAT);
    target.push_back(source);
    target.push_back(0x0D);
    target.push_back(0x0A);
    return target.size();
}

int32 Mikuni::unpack(const byte_array &source, byte_array &target) {
    if (source.size() <= 0) {
        return -1;
    }
    
    target.clear();
    target.push_back(source.data() + 1, source.size() - 2);
    return target.size();
}

}
