/* 
 * File:   jm_protocol.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:24
 */

#ifndef JM_PROTOCOL_HPP
#define	JM_PROTOCOL_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_byte_array.hpp>

namespace jm {
class protocol {
public:
    virtual int32 pack(const byte_array &source, byte_array &target) = 0;
    virtual int32 unpack(const byte_array &source, byte_array &target) = 0;
};
}


#endif	/* JM_PROTOCOL_HPP */

