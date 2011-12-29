#include "jm_kline_protocol.hpp"

namespace jm {

kline_protocol::kline_protocol() : _target_address(0), _source_address(0) {

}

void kline_protocol::set_address(int32 target_address, int32 source_address) {
    _target_address = target_address;
    _source_address = source_address;
}

}
