/* 
 * File:   jm_comm.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:14
 */

#ifndef JM_COMM_HPP
#define	JM_COMM_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_byte_array.hpp>
#include <boost/smart_ptr.hpp>

namespace jm {

template <typename P>
class comm {
public:
    virtual boost::shared_ptr<P> get_protocol() = 0;
    virtual int32 send_one_frame(const byte_array& data) = 0;
    virtual int32 send_frames(const byte_array& data) = 0;
    virtual int32 read_one_frame(byte_array& data) = 0;
    virtual int32 read_frames(byte_array& data) = 0;
    virtual int32 send_and_recv(const byte_array &send, byte_array &recv) = 0;
    virtual int32 start_keep_link(bool run) = 0;
    virtual int32 set_keep_link(const byte_array &data) = 0;
    virtual int32 set_timeout(int32 tx_b2b, int32 rx_b2b, int32 tx_f2f, int32 rx_f2f, int32 total) = 0;
};

}


#endif	/* JM_COMM_HPP */

