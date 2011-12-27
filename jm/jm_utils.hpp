/* 
 * File:   jm_utils.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午2:52
 */

#ifndef JM_UTILS_HPP
#define	JM_UTILS_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_types.hpp>

namespace jm {

template<typename T>
inline uint8 high_byte(T value) {
    return static_cast<uint8>((static_cast<uint16>(value) >> 8) & 0xFF);
}

template<typename T>
inline uint8 low_byte(T value) {
    return static_cast<uint8>(static_cast<uint16>(value) & 0xFF);
}

template<typename T>
inline uint16 high_word(T value) {
    return static_cast<uint16>((static_cast<uint32>(value) >> 16) & 0xFFFF);
}

template<typename T>
inline uint16 low_word(T value) {
    return static_cast<uint16>(static_cast<uint32>(value) & 0xFFFF);
}

}


#endif	/* JM_UTILS_HPP */

