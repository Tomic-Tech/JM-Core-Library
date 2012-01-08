/* 
 * File:   jm_kwp1281.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午3:12
 */

#ifndef JM_KWP1281_HPP
#define	JM_KWP1281_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jm_kline_protocol.hpp>

namespace jm {

class JMCORE_API kwp1281 : public kline_protocol {
public:
    static const uint8 FRAME_END = 0x03;
protected:
    uint8 _frame_counter;
public:
    kwp1281();
    size_type pack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
    size_type unpack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
protected:
    uint8 frame_counter_increment();
};

typedef boost::shared_ptr<kwp1281> kwp1281_ptr;

}


#endif	/* JM_KWP1281_HPP */

