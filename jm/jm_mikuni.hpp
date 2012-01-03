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

class JMCORE_API mikuni : public kline_protocol {
public:
    static const int32 HEAD_FORMAT = 0x48;
    size_type pack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
    size_type unpack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
};

typedef boost::shared_ptr<mikuni> mikuni_ptr;

}


#endif	/* JM_MIKUNI_HPP */

