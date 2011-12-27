/* 
 * File:   jm_mikuni.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午4:21
 */

#ifndef JM_MIKUNI_HPP
#define	JM_MIKUNI_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_kline_protocol.hpp>

namespace jm {

class Mikuni : public kline_protocol {
public:
    static const int32 HEAD_FORMAT = 0x48;
    int32 pack(const byte_array &source, byte_array &target);
    int32 unpack(const byte_array &source, byte_array &target);
};

}


#endif	/* JM_MIKUNI_HPP */

