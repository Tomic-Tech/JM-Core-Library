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

#include <boost/smart_ptr.hpp>
#include <jm/jm_byte_array.hpp>
#include <jm/jm_error.hpp>

namespace jm {

template <typename P>
class JMCORE_API comm {
public:
    virtual boost::shared_ptr<P> get_protocol() = 0;
    virtual size_type send_one_frame(const uint8 *data, size_type offset, size_type count) = 0;

    virtual size_type send_one_frame(const byte_array& data) {
        return send_one_frame(data.data(), 0, data.size());
    }
    virtual size_type send_frames(const uint8 *data, size_type offset, size_type count) = 0;

    virtual size_type send_frames(const byte_array& data) {
        return send_frames(data.data(), 0, data.size());
    }
    virtual size_type read_one_frame(uint8 *data, size_type offset) = 0;

    virtual size_type read_one_frame(byte_array& data) {
        uint8 buff[256];
        data.clear();
        size_type ret = read_one_frame(buff, 0);
        if (ret > 0) {
            data.push_back(buff, 0, ret);
        }
        return ret;
    }
    virtual size_type read_frames(uint8 *data, size_type offset) = 0;

    virtual size_type read_frames(byte_array& data) {
        uint8 buff[0xFFFF];
        data.clear();
        size_type ret = read_frames(buff, 0);
        if (ret > 0) {
            data.push_back(buff, 0, ret);
        }
        return ret;
    }
    virtual size_type send_and_recv(const uint8 *send, size_type send_offset, size_type send_count,
            uint8 *recv, size_type recv_offset) = 0;

    virtual size_type send_and_recv(const byte_array &send, byte_array &recv) {
        uint8 buff[0xFFFF];
        recv.clear();
        size_type ret = send_and_recv(send.data(), 0, send.size(), buff, 0);
        if (ret > 0) {
            recv.push_back(buff, 0, ret);
        }
        return ret;
    }
    virtual error_code start_keep_link(bool run) = 0;
    virtual error_code set_keep_link(const uint8 *data, size_type offset, size_type count) = 0;

    virtual error_code set_keep_link(const byte_array &data) {
        return set_keep_link(data.data(), 0, data.size());
    }
    virtual error_code set_timeout(int32 tx_b2b, int32 rx_b2b, int32 tx_f2f, int32 rx_f2f, int32 total) = 0;
};

}


#endif	/* JM_COMM_HPP */

