/* 
 * File:   jm_protocol_type.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:12
 */

#ifndef JM_PROTOCOL_TYPE_HPP
#define	JM_PROTOCOL_TYPE_HPP

#ifdef _MSC_VER
#pragma once
#endif

namespace jm {

enum protocol_type {
    J1850VPW = 0,
    J1850PWM = 1,
    ISO9141_2 = 2,
    ISO14230 = 3,
    ISO15765 = 4,
    ALDL160 = 5,
    ALDL8192 = 6,
    KWP1281 = 7,
    MIKUNI = 8
};
}


#endif	/* JM_PROTOCOL_TYPE_HPP */

