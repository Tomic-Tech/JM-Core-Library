#ifndef __JM_UTILS_H__
#define __JM_UTILS_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/cstdint.hpp>

namespace JM
{

template<typename T>
inline boost::uint8_t high_byte(const T &value)
{
    return static_cast<boost::uint8_t>((static_cast<boost::uint16_t>(value) >> 8) & 0xFF);
}

template<typename T>
inline boost::uint8_t low_byte(const T &value)
{
    return static_cast<boost::uint8_t>(static_cast<boost::uint16_t>(value) & 0xFF);
}

template<typename T>
inline boost::uint16_t high_word(const T &value)
{
    return static_cast<boost::uint16_t>((static_cast<boost::uint32_t>(value) >> 16) & 0xFFFF);
}

template<typename T>
inline boost::uint16_t low_word(const T &value)
{
    return static_cast<boost::uint16_t>(static_cast<boost::uint32_t>(value) & 0xFFFF);
}

template<typename T>
inline boost::uint32_t high_dword(const T &value)
{
    return static_cast<boost::uint32_t>((static_cast<boost::uint64_t>(value) >> 32) & 0xFFFFFFFF);
}

template<typename T>
inline boost::uint32_t low_dword(const T &value)
{
    return static_cast<boost::uint32_t>(static_cast<boost::uint64_t>(value) & 0xFFFFFFFF);
}

template<typename String>
char* std_string_to_char_string(const String &str)
{
    char *ret = (char *)malloc(str.length() + 1);
    strncpy(ret, str.c_str(), str.length());
    return ret;
}

}

#endif
