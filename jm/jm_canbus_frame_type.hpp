/* 
 * File:   jm_canbus_frame_type.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:33
 */

#ifndef JM_CANBUS_FRAME_TYPE_HPP
#define	JM_CANBUS_FRAME_TYPE_HPP

#ifdef _MSC_VER
#pragma once
#endif

namespace jm {
enum canbus_frame_type {
    Data = 0x00,
    Remote = 0x40
};
}


#endif	/* JM_CANBUS_FRAME_TYPE_HPP */

