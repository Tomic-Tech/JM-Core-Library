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
    virtual size_t pack(const uint8 *src, size_t src_offset, size_t count,
            uint8 *tar, size_t tar_offset) = 0;
    virtual size_t unpack(const uint8 *src, size_t src_offset, size_t count,
            uint8 *tar, size_t tar_offset) = 0;
    virtual size_t pack(const byte_array &src, byte_array &tar);
    virtual size_t unpack(const byte_array &src, byte_array &tar);
    virtual size_t send_one_frame(const uint8 *data, size_t offset, size_t count) = 0;
    virtual size_t send_one_frame(const byte_array &data);
    virtual size_t read_one_frame(uint8 *data, size_t offset) = 0;
    virtual size_t read_one_frame(byte_array &data);
};

typedef boost::shared_ptr<protocol> protocol_ptr;

}


#endif	/* JM_PROTOCOL_HPP */

