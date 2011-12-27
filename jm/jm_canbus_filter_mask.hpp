/* 
 * File:   jm_canbus_filter_mask.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:32
 */

#ifndef JM_CANBUS_FILTER_MASK_HPP
#define	JM_CANBUS_FILTER_MASK_HPP

#ifdef _MSC_VER
#pragma once
#endif

namespace jm {
enum canbus_filter_mask {
    Enable = 0x0F,
    Disable = 0x00
};
}


#endif	/* JM_CANBUS_FILTER_MASK_HPP */

