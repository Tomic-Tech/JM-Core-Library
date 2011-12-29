#include "jm_v1_shared.hpp"

namespace jm {
namespace v1 {
v1_shared::v1_shared() {
    connector = OBDII_16;
    buff_id = 0;
    last_error = 0;
    is_db_20 = false;
    is_do_now = true;
    req_byte_to_byte = 0;
    req_wait_time = 0;
    res_wait_time = 0;
    res_byte_to_byte = 0;
    next_address = 0;
}

}
}