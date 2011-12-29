/* 
 * File:   jm_port.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 上午11:41
 */

#ifndef JM_COMMBOX_PORT_HPP
#define	JM_COMMBOX_PORT_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <deque>
#include <boost/thread.hpp>
#include <jm_types.hpp>
#include <jm_timer.hpp>
#include <jm_error.hpp>

namespace jm {

class commbox_port {
public:
    commbox_port();
    ~commbox_port();
    error_code set_read_timeout(int64 microseconds);
    error_code set_write_timeout(int64 microseconds);
    size_t bytes_available();
    error_code discard_out_buffer();
    error_code discard_in_buffer();
    size_t read(uint8 *data, size_t offset, size_t count);
    size_t write(const uint8 *data, size_t offset, size_t count);
public: // below is only for framework, users must not call or get.
    void push_in_deque(const uint8 *data, size_t offset, size_t count);
    size_t out_deque_size();
    size_t pop_out_deque(uint8 *data, size_t offset, size_t count);
private:

    commbox_port(const commbox_port& other) {
    }

    commbox_port& operator=(const commbox_port& other) {
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

typedef boost::shared_ptr<commbox_port> commbox_port_ptr;

}



#endif	/* JM_PORT_HPP */

