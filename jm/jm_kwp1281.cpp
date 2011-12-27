#include "jm_kwp1281.hpp"
#include "jm_utils.hpp"

namespace jm {

kwp1281::kwp1281() {
    _frame_counter = 0;
}

int32 kwp1281::pack(const byte_array &source, byte_array &target) {
    target.clear();
    if (source.size() <= 0) {
        return -1;
    }
    target.push_back(low_byte(source.size() + 20));
    target.push_back(frame_counter_increment());
    target.push_back(source);
    target.push_back(FRAME_END);
    return target.size();
}

int32 kwp1281::unpack(const byte_array &source, byte_array &target) {
    if ((source.size() - 2) <= 0) {
        return -1;
    }
    
    target.clear();
    target.push_back(source.data() + 1, source.size() - 2);
    return target.size();
}

uint8 kwp1281::frame_counter_increment() {
    return ++_frame_counter;
}

}
