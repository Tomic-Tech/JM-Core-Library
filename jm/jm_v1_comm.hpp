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
#include <jm_comm.hpp>
#include <jm_v1_box.hpp>

namespace jm {

template <typename P>
class v1_comm : public comm<P> {
private:
    boost::shared_ptr<v1_box> _box;
    boost::shared_ptr<P> _protocol;

public:
    v1_comm(boost::shared_ptr<v1_box> box, boost::shared_ptr<P> protocol) {
        _box = box;
        _protocol = protocol;
    }
    
    int32 send_one_frame(const byte_array &data) {
        return _protocol->send_one_frame(data);
    }
    
    int32 send_frames(const byte_array &data) {
        return _protocol->send_frames(data);
    }
    
    int32 read_one_frame(byte_array &data) {
        return _protocol->read_one_frame(data);
    }
    
    int32 read_frames(byte_array &data) {
        return _protocol->read_frames(data);
    }
    
    int32 send_and_recv(const byte_array &send, byte_array &recv) {
        int32 times = v1_c168_d::REPLAYTIMES;
        while (times--) {
            if (send_frames(send) > 0) {
                return read_frames(recv);
            }
        }
        return -1;
    }
    
    int32 set_timeout(int32 tx_b2b, int32 rx_b2b, int32 rx_f2f, int32 total) {
        return 0;
    }
    
    int32 start_keep_link(bool run) {
        if (!_box->keep_link(run)) {
            return -1;
        }
        return 0;
    }
    
    int32 set_keep_link(const byte_array &data) {
        return _protocol->set_keep_link(data);  
    }
    
    boost::shared_ptr<P> get_protocol() {
        return _protocol;
    }
};

}


#endif	/* JM_V1_COMM_HPP */

