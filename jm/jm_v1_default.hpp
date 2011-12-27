/* 
 * File:   jm_v1_default.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月25日, 下午11:03
 */

#ifndef JM_V1_DEFAULT_HPP
#define	JM_V1_DEFAULT_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <jm_v1_box.hpp>
#include <jm_v1_shared.hpp>

namespace jm {

template <typename T>
class v1_default {
private:
    boost::shared_ptr<v1_box> _box;
    boost::shared_ptr<v1_shared> _shared;
    boost::shared_ptr<T> _protocol;
public:
    v1_default(const boost::shared_ptr<v1_box> box, boost::shared_ptr<v1_shared> shared, boost::shared_ptr<T> protocol) {
        _box = box;
        _shared = shared;
        _protocol = protocol;
    }
    
    int32 send_one_frame(const uint8 *data, int32 count, bool need_recv) {
        _shared->buff_id = 0;
        if (!_box->new_batch(_shared->buff_id)) {
            return -1;
        }
        byte_array send_buff;
        _protocol.pack(send_buff, byte_array(data, count));
        if (send_buff.empty()) {
            return -1;
        }
        if (need_recv) {
            if (!_box->send_out_data(send_buff.data(), 0, send_buff.size())
                    || !_box->run_receive(v1_shared::RECEIVE)
                    || !_box->end_batch()
                    || !_box->run_batch(&_shared->buff_id, 1, false)) {
                return -1;
            }
        } else {
            if (!_box->send_out_data(send_buff.data(), 0, send_buff.size())
                    || !_box->end_batch()
                    || !_box->run_batch(&_shared->buff_id, 1, false)) {
                return -1;
            }
        }
        _protocol->finish_execute(!need_recv);
        return send_buff.size();
    }
    
    int32 set_keep_link(const uint8 *data, int32 count) {
        if (!_box->new_batch(v1_shared::LINKBLOCK)) {
            return -1;
        }
        if (!_box->send_out_data(data, 0, count)
                || !_box->end_batch()) {
            return -1;
        }
        return count;
    }
};

}


#endif	/* JM_V1_DEFAULT_HPP */

