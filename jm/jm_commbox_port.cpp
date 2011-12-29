#include "jm_commbox_port.hpp"

namespace jm {

commbox_port::commbox_port()
: _read_timeout(500000)
, _write_timeout(500000) {

}

commbox_port::~commbox_port() {

}

error_code commbox_port::set_read_timeout(int64 microseconds) {
    _read_timeout = microseconds;
}

error_code commbox_port::set_write_timeout(int64 microseconds) {
    _write_timeout = microseconds;
}

size_t commbox_port::bytes_available() {
    return _in_deque.size();
}

error_code commbox_port::discard_out_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_out_mutex);
    _out_deque.clear();
    return 0;
}

error_code commbox_port::discard_in_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_in_mutex);
    _in_deque.clear();
    return 0;
}

size_t commbox_port::read(uint8 *data, size_t offset, size_t count) {
    if (_read_timeout <= 0) {
        while (count > _in_deque.size());
        boost::recursive_mutex::scoped_lock scoped_lock(_in_mutex);
        for (int i = 0; i < count; i++) {
            data[i + offset] = _in_deque.front();
            _in_deque.pop_front();
        }
        return count;
    }
    _timer.set_timeout(_read_timeout);
    while (!_timer.is_timeout()) {
        if (_in_deque.size() >= count) {
            boost::recursive_mutex::scoped_lock scoped_lock(_in_mutex);
            for (int i = 0; i < count; i++) {
                data[i + offset] = _in_deque.front();
                _in_deque.pop_front();
            }
            return count;
        }
    }
    return -1;
}

size_t commbox_port::write(const uint8 *data, size_t offset, size_t count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_out_mutex);
    for (int i = 0; i < count; i++) {
        _out_deque.push_back(data[i + offset]);
    }
    return count;
}

void commbox_port::push_in_deque(const uint8 *data, size_t offset, size_t count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_in_mutex);
    for (int i = 0; i < count; i++) {
        _in_deque.push_back(data[i + offset]);
    }
}

size_t commbox_port::out_deque_size() {
    return _out_deque.size();
}

size_t commbox_port::pop_out_deque(uint8 *data, size_t offset, size_t count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_out_mutex);
    if (count > _out_deque.size()) {
        int32 size = _out_deque.size();
        for (int i = 0; i < size; i++) {
            data[i + offset] = _out_deque.front();
            _out_deque.pop_front();
        }
        return size;
    }
    for (int i = 0; i < count; i++) {
        data[i + offset] = _out_deque.front();
        _out_deque.pop_front();
    }
    return count;
}

}
