/* 
 * File:   jm_canbus_id_mode.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:39
 */

#ifndef JM_CANBUS_ID_MODE_HPP
#define	JM_CANBUS_ID_MODE_HPP

#ifdef _MSC_VER
#pragma once
#endif

namespace jm {
enum canbus_id_mode {
    STD = 0x00,
    EXT = 0x80
};
}


#endif	/* JM_CANBUS_ID_MODE_HPP */

