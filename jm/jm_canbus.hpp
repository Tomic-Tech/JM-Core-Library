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

class JMCORE_API canbus : public protocol {
protected:
    int32 _target_id;
    canbus_baud _baud;
    canbus_id_mode _id_mode;
    canbus_filter_mask _filter_mask;
    canbus_frame_type _frame_type;
    int32 _high;
    int32 _low;
    int32_vector _id_array;
    int32 _flow_control[8];

public:
    canbus();
    virtual size_type pack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
    virtual size_type unpack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
    virtual error_code set_lines(int32 high, int32 low) = 0;
    virtual error_code set_filter(const int32 *id_array, int32 count) = 0;
    virtual error_code set_options(int32 id, canbus_baud baud, canbus_id_mode id_mode, canbus_filter_mask mask, canbus_frame_type frame) = 0;
};

typedef boost::shared_ptr<canbus> canbus_ptr;
}

#endif	/* JM_CANBUS_HPP */

