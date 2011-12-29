/* 
 * File:   jm_canbus.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:40
 */

#ifndef JM_CANBUS_HPP
#define	JM_CANBUS_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <jm_protocol.hpp>
#include <jm_canbus_baud.hpp>
#include <jm_canbus_filter_mask.hpp>
#include <jm_canbus_frame_type.hpp>
#include <jm_canbus_id_mode.hpp>
#include <jm_error.hpp>

namespace jm {

class canbus : public protocol {
protected:
    int32 _target_id;
    canbus_baud _baud;
    canbus_id_mode _id_mode;
    canbus_filter_mask _filter_mask;
    canbus_frame_type _frame_type;
    int32 _high;
    int32 _low;
    std::vector<int32> _id_array;
    int32 _flow_control[8];

public:
    canbus();
    virtual size_t pack(const uint8 *src, size_t src_offset, size_t count,
            uint8 *tar, size_t tar_offset);
    virtual size_t unpack(const uint8 *src, size_t src_offset, size_t count,
            uint8 *tar, size_t tar_offset);
    virtual error_code set_lines(int32 high, int32 low) = 0;
    virtual error_code set_filter(int32 *id_array, int32 count) = 0;
    virtual error_code set_options(int32 id, canbus_baud baud, canbus_id_mode id_mode, canbus_filter_mask mask, canbus_frame_type frame) = 0;
};

typedef boost::shared_ptr<canbus> canbus_ptr;
}

#endif	/* JM_CANBUS_HPP */

