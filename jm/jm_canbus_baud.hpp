/* 
 * File:   jm_canbus_baud.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:30
 */

#ifndef JM_CANBUS_BAUD_HPP
#define	JM_CANBUS_BAUD_HPP

#ifdef _MSC_VER
#pragma once
#endif

namespace jm {
enum canbus_baud {
    B1000K = 1000000,
    B800K = 800000,
    B666K = 666000,
    B500K = 500000,
    B400K = 400000,
    B250K = 250000,
    B200K = 200000,
    B125K = 125000,
    B100K = 100000,
    B80K = 80000,
    B50K = 50000,
    B40K = 40000,
    B20K = 20000,
    B10K = 10000,
    B5K = 5000
};
}


#endif	/* JM_CANBUS_BAUD_HPP */

