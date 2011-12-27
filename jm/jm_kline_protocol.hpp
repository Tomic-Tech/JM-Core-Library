/* 
 * File:   jm_kline_protocol.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:27
 */

#ifndef JM_KLINE_PROTOCOL_HPP
#define	JM_KLINE_PROTOCOL_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_protocol.hpp>

namespace jm {

class kline_protocol : public protocol {
protected:
    int32 _target_address;
    int32 _source_address;

public:
    kline_protocol();
    void set_address(int32 target_address, int32 source_address) {
        _target_address = target_address;
        _source_address = source_address;
    }

    virtual int32 addr_init(int32 addr_code) = 0;
    virtual int32 set_lines(int32 com_line, bool l_line) = 0;
};
}


#endif	/* JM_KLINE_PROTOCOL_HPP */
