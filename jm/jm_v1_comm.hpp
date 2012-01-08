/* 
 * File:   jm_v1_comm.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午5:21
 */

#ifndef JM_V1_COMM_HPP
#define	JM_V1_COMM_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <jm/jm_comm.hpp>
#include <jm/jm_error.hpp>
#include <jm/jm_v1_box.hpp>

namespace jm {
namespace v1 {
template <typename BOX, typename P>
class v1_comm : public comm<P> {
private:
    boost::shared_ptr<BOX> _box;
    boost::shared_ptr<P> _protocol;

public:
    v1_comm(const boost::shared_ptr<BOX> &box, const boost::shared_ptr<P> &protocol) {
        _box = box;
        _protocol = protocol;
    }
    
    size_type send_one_frame(const uint8* data, size_type offset, size_type count) {
        return _protocol->send_one_frame(data, offset, count);
    }
    
    size_type send_frames(const uint8 *data, size_type offset, size_type count) {
        return _protocol->send_frames(data, offset, count);
    }
    
    size_type read_one_frame(uint8 *data, size_type offset) {
        return _protocol->read_one_frame(data, offset);
    }
    
    size_type read_frames(uint8 *data, size_type offset) {
        return _protocol->read_frames(data, offset);
    }
    
    size_type send_and_recv(const uint8 *send, size_type send_offset, size_type count,
            uint8 *recv, size_type recv_offset) {
        int32 times = v1_c168_d::REPLAYTIMES;
        while (times--) {
            if (send_frames(send, send_offset, count) > 0) {
                return read_frames(recv, recv_offset);
            }
        }
        return -1;
    }
    
    error_code set_timeout(int32 tx_b2b, int32 rx_b2b, int32 tx_f2f, int32 rx_f2f, int32 total) {
        return error::success;
    }
    
    error_code start_keep_link(bool run) {
        if (!_box->keep_link(run)) {
            return error::generic_error;
        }
        return error::success;
    }
    
    error_code set_keep_link(const uint8 *data, size_type offset, size_type count) {
        return _protocol->set_keep_link(data, offset, count);  
    }
    
    boost::shared_ptr<P> get_protocol() {
        return _protocol;
    }
};

}
}


#endif	/* JM_V1_COMM_HPP */

