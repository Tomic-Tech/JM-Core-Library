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

template <typename BOX, typename T>
class v1_default {
private:
    boost::shared_ptr<BOX> _box;
    v1_shared_ptr _shared;
    T* _protocol;
public:

    v1_default(const boost::shared_ptr<BOX> &box, const v1_shared_ptr &shared, T* protocol)
    : _box(box)
    , _shared(shared)
    , _protocol(protocol) {
    }

    size_t send_one_frame(const uint8 *data, size_t offset, size_t count, bool need_recv) {
        _shared->buff_id = 0;
        if (!_box->new_batch(_shared->buff_id)) {
            return 0;
        }
        uint8 send_buff[256];
        size_t length = _protocol->pack(data, offset, count, send_buff, 0);

        if (length <= 0) {
            return 0;
        }
        if (need_recv) {
            if (!_box->send_out_data(send_buff, 0, length)
                    || !_box->run_receive(BOX::D::RECEIVE)
                    || !_box->end_batch()
                    || !_box->run_batch(&_shared->buff_id, 1, false)) {
                return 0;
            }
        } else {
            if (!_box->send_out_data(send_buff, 0, length)
                    || !_box->end_batch()
                    || !_box->run_batch(&_shared->buff_id, 1, false)) {
                return 0;
            }
        }
        _protocol->finish_execute(!need_recv);
        return length;
    }

    error_code set_keep_link(const uint8 *data, size_t offset, size_t count) {
        if (!_box->new_batch(BOX::D::LINKBLOCK)) {
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

