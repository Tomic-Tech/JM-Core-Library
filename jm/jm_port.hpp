/* 
 * File:   jm_port.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 上午11:41
 */

#ifndef JM_PORT_HPP
#define	JM_PORT_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_types.hpp>
#include <deque>
#include <boost/thread.hpp>
#include <jm_timer.hpp>

namespace jm {

class port {
public:
    port();
    ~port();
    int32 set_read_timeout(int64 microseconds);
    int32 set_write_timeout(int64 microseconds);
    int32 bytes_available();
    int32 discard_out_buffer();
    int32 discard_in_buffer();
    int32 read(uint8 *data, int32 offset, int32 count);
    int32 write(const uint8 *data, int32 offset, int32 count);
public: // below is only for framework, users must not call or get.
    void push_in_deque(const uint8 *data, int32 offset, int32 count);
    int32 out_deque_size();
    int32 pop_out_deque(uint8 *data, int32 count);
private:

    port(const port& other) {
    }

    port& operator=(const port& other) {
        return *this;
    }
private:
    int64 _read_timeout;
    int64 _write_timeout;
    std::deque<uint8> _in_deque;
    std::deque<uint8> _out_deque;
    boost::recursive_mutex _in_mutex;
    boost::recursive_mutex _out_mutex;
    timer _timer;
};
}



#endif	/* JM_PORT_HPP */

