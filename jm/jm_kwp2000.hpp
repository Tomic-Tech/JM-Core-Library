/* 
 * File:   jm_kwp2000.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午3:20
 */

#ifndef JM_KWP2000_HPP
#define	JM_KWP2000_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_kline_protocol.hpp>
#include <jm_kwp_mode.hpp>

namespace jm {

class JMCORE_API kwp2000 : public kline_protocol {
public:
    static const int32 KWP8XHeaderLength = 3;
    static const int32 KWPCXHeaderLength = 3;
    static const int32 KWP80HeaderLength = 4;
    static const int32 KWPXXHeaderLength = 1;
    static const int32 KWP00HeaderLength = 2;
    static const int32 KWPChecksumLength = 1;
    static const int32 KWPMaxDataSize = 128;
protected:
    kwp_mode _mode;
    kwp_mode _msg_mode;
    kwp_mode _link_mode;
    int32 _baud;

public:
    kwp2000();
    void set_options(kwp_mode msg_mode, kwp_mode link_mode, int32 baud);
    size_type pack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
    size_type unpack(const uint8 *src, size_type src_offset, size_type count,
            uint8 *tar, size_type tar_offset);
    virtual error_code fast_init(const uint8 *data, size_type offset, size_type count) = 0;
    virtual error_code fast_init(const byte_array &data);
};

typedef boost::shared_ptr<kwp2000> kwp2000_ptr;

}

#endif	/* JM_KWP2000_HPP */

