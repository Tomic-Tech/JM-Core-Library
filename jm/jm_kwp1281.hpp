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

#include <jm_kline_protocol.hpp>

namespace jm {
class kwp1281 : public kline_protocol {
public:
    static const uint8 FRAME_END = 0x03;
protected:
    uint8 _frame_counter;
public:
    kwp1281();
    int32 pack(const byte_array &source, byte_array &target);
    int32 unpack(const byte_array &source, byte_array &target);
protected:
    uint8 frame_counter_increment();
};
}


#endif	/* JM_KWP1281_HPP */

