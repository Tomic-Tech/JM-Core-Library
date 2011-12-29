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
namespace v1 {

template <typename T>
class v1_default {
private:
    v1_box_ptr _box;
    v1_shared_ptr _shared;
    boost::shared_ptr<T> _protocol;
public:

    v1_default(const v1_box_ptr box, v1_shared_ptr shared, boost::shared_ptr<T> protocol) {
        _box = box;
        _shared = shared;
        _protocol = protocol;
    }

    size_t send_one_frame(const uint8 *data, size_t offset, size_t count, bool need_recv) {
        _shared->buff_id = 0;
        if (!_box->new_batch(_shared->buff_id)) {
            return -1;
        }
        byte_array send_buff;
        _protocol.pack(send_buff, byte_array(data + offset, count));
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

    error_code set_keep_link(const uint8 *data, size_t offset, size_t count) {
        if (!_box->new_batch(v1_shared::LINKBLOCK)) {
            return error::generic_error;
        }
        if (!_box->send_out_data(data, offset, count)
                || !_box->end_batch()) {
            return -1;
        }
        return count;
    }
};

}
}


#endif	/* JM_V1_DEFAULT_HPP */

