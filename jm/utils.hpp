#ifndef __JM_UTILS_HPP__
#define __JM_UTILS_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/cstdint.hpp>

namespace JM
{
template<typename T>
inline boost::uint8_t HighByte(const T &value)
{
    return static_cast<boost::uint8_t>((static_cast<boost::uint16_t>(value) >> 8) & 0xFF);
}

template<typename T>
inline boost::uint8_t LowByte(const T &value)
{
    return static_cast<boost::uint8_t>(static_cast<boost::uint16_t>(value) & 0xFF);
}

template<typename T>
inline boost::uint16_t HighWord(const T &value)
{
    return static_cast<boost::uint16_t>((static_cast<boost::uint32_t>(value) >> 16) & 0xFFFF);
}

template<typename T>
inline boost::uint16_t LowWord(const T &value)
{
    return static_cast<boost::uint16_t>(static_cast<boost::uint32_t>(value) & 0xFFFF);
}

template<typename T>
inline boost::uint32_t HighDWord(const T &value)
{
    return static_cast<boost::uint32_t>((static_cast<boost::uint64_t>(value) >> 32) & 0xFFFFFFFF);
}

template<typename T>
inline boost::uint32_t LowDWord(const T &value)
{
    return static_cast<boost::uint32_t>(static_cast<boost::uint64_t>(value) & 0xFFFFFFFF);
}

template<typename STRING>
char* stdStringToCharString(const STRING &str)
{
    char *ret = (char *)malloc(str.length() + 1);
    strncpy(ret, str.c_str(), str.length());
    return ret;
}

}

#endif