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

class mikuni : public kline_protocol {
public:
    static const int32 HEAD_FORMAT = 0x48;
    size_t pack(const uint8 *src, size_t src_offset, size_t count,
            uint8 *tar, size_t tar_offset);
    size_t unpack(const uint8 *src, size_t src_offset, size_t count,
            uint8 *tar, size_t tar_offset);
};

typedef boost::shared_ptr<mikuni> mikuni_ptr;

}


#endif	/* JM_MIKUNI_HPP */

