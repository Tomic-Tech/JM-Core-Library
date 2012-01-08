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

#include <jm/jm_types.hpp>
#include <boost/smart_ptr.hpp>
#include <jm/jm_byte_array.hpp>

namespace jm {

class commbox_port_private;
class JMCORE_API commbox_port {
public:
    commbox_port();
    ~commbox_port();
    error_code set_read_timeout(int64 microseconds);
    error_code set_write_timeout(int64 microseconds);
    size_type bytes_available();
    error_code discard_out_buffer();
    error_code discard_in_buffer();
    size_type read(uint8 *data, size_type offset, size_type count);
    size_type write(const uint8 *data, size_type offset, size_type count);
public: // below is only for framework, users must not call or get.
    void push_in_deque(const uint8 *data, size_type offset, size_type count);
	bool out_deque_available();
    //size_type out_deque_size();
    //size_type pop_out_deque(uint8 *data, size_type offset, size_type count);
	bool pop_out_deque(byte_array &data);
private:

    commbox_port(const commbox_port& other) {
    }

    commbox_port& operator=(const commbox_port& other) {
        return *this;
    }
private:
	commbox_port_private *_pri;
};

typedef boost::shared_ptr<commbox_port> commbox_port_ptr;

}



#endif	/* JM_PORT_HPP */

