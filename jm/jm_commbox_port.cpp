#include "jm_commbox_port.hpp"
#include <deque>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "jm_timer.hpp"
#include "jm_error.hpp"
#include "jm_log.hpp"
#include "jm_byte_array.hpp"

namespace jm {

class commbox_port_private {
private:
    friend class commbox_port;
    int64 _read_timeout;
    int64 _write_timeout;
    std::deque<uint8> _in_deque;
    std::deque<byte_array> _out_deque;
    boost::mutex _in_mutex;
    boost::recursive_mutex _out_mutex;
    boost::condition_variable _in_cond;
    timer _timer;

    commbox_port_private()
    : _read_timeout(500000)
    , _write_timeout(500000) {
    }
};

commbox_port::commbox_port()
: _pri(new commbox_port_private()) {

}

commbox_port::~commbox_port() {
    delete _pri;
}

error_code commbox_port::set_read_timeout(int64 microseconds) {
    _pri->_read_timeout = microseconds;
    return error::success;
}

error_code commbox_port::set_write_timeout(int64 microseconds) {
    _pri->_write_timeout = microseconds;
    return error::success;
}

size_type commbox_port::bytes_available() {
    return _pri->_in_deque.size();
}

error_code commbox_port::discard_out_buffer() {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_out_mutex);
    _pri->_out_deque.clear();
    return 0;
}

error_code commbox_port::discard_in_buffer() {
    boost::lock_guard<boost::mutex> lock(_pri->_in_mutex);
    _pri->_in_deque.clear();
    return 0;
}

size_type commbox_port::read(uint8 *data, size_type offset, size_type count) {
    if (count <= _pri->_in_deque.size()) {
        boost::mutex::scoped_lock scoped_lock(_pri->_in_mutex);
        for (size_type i = 0; i < count; i++) {
            data[i + offset] = _pri->_in_deque.front();
            _pri->_in_deque.pop_front();
        }
        return count;
    } else {
        if (_pri->_read_timeout <= 0) {
            boost::unique_lock<boost::mutex> lock(_pri->_in_mutex);
            while (_pri->_in_deque.size() < count) {
                _pri->_in_cond.wait(lock);
            }
            for (size_type i = 0; i < count; i++) {
                data[i + offset] = _pri->_in_deque.front();
                _pri->_in_deque.pop_front();
            }
            return count;
        } else {
            boost::unique_lock<boost::mutex> lock(_pri->_in_mutex);
            size_type len = 0;
            while (true) {
                if (_pri->_in_cond.timed_wait(lock, boost::get_system_time() + boost::posix_time::microsec(_pri->_read_timeout))) {
                    size_type size = _pri->_in_deque.size();
                    if (size >= count) {
                        for (size_type i = 0; i < count; i++) {
                            data[i + offset + len] = _pri->_in_deque.front();
                            _pri->_in_deque.pop_front();
                        }
                        len += count;
                        return len;
                    } else {
                        for (size_type i = 0; i < size; i++) {
                            data[i + offset + len] = _pri->_in_deque.front();
                            _pri->_in_deque.pop_front();
                        }
                        len += size;
                        count -= size;
                    }
                } else {
                    return 0;
                }
            }
        }
    }
    return 0;
}

size_type commbox_port::write(const uint8 *data, size_type offset, size_type count) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_out_mutex);
    _pri->_out_deque.push_back(byte_array(data, offset, count));
    return count;
}

void commbox_port::push_in_deque(const uint8 *data, size_type offset, size_type count) {
    boost::lock_guard<boost::mutex> lock(_pri->_in_mutex);
    for (size_type i = 0; i < count; i++) {
        _pri->_in_deque.push_back(data[i + offset]);
    }
    _pri->_in_cond.notify_all();
}

bool commbox_port::out_deque_available() {
    return !_pri->_out_deque.empty();
}

//size_type commbox_port::out_deque_size() {
//    return _pri->_out_deque.size();
//}
//
//size_type commbox_port::pop_out_deque(uint8 *data, size_type offset, size_type count) {
//    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_out_mutex);
//    if (count > _pri->_out_deque.size()) {
//        int32 size = _pri->_out_deque.size();
//        for (int i = 0; i < size; i++) {
//            data[i + offset] = _pri->_out_deque.front();
//            _pri->_out_deque.pop_front();
//        }
//        return size;
//    }
//    for (size_type i = 0; i < count; i++) {
//        data[i + offset] = _pri->_out_deque.front();
//        _pri->_out_deque.pop_front();
//    }
//    return count;
//}

bool commbox_port::pop_out_deque(byte_array &data) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_out_mutex);
    if (_pri->_out_deque.empty())
        return false;
    data = _pri->_out_deque.front();
    _pri->_out_deque.pop_front();
    return true;
}

}
