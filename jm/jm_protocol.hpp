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

class JMCORE_API protocol {
public:
    virtual size_type pack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset) = 0;
    virtual size_type unpack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset) = 0;
    virtual size_type pack(const byte_array &src, byte_array &tar);
    virtual size_type unpack(const byte_array &src, byte_array &tar);
    virtual size_type send_one_frame(const uint8 *data, size_type offset, size_type count) = 0;
    virtual size_type send_one_frame(const byte_array &data);
    virtual size_type read_one_frame(uint8 *data, size_type offset) = 0;
    virtual size_type read_one_frame(byte_array &data);
};

typedef boost::shared_ptr<protocol> protocol_ptr;

}


#endif	/* JM_PROTOCOL_HPP */

