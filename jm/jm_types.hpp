#ifndef JM_TYPES_HPP
#define JM_TYPES_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_macros.hpp>
#include <iostream>
#include <vector>

namespace jm {

typedef unsigned char uchar;
typedef unsigned char uchar;
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long ulong;

typedef int32 error_code;
typedef std::size_t size_type;

typedef void* pointer;

typedef std::vector<int32> int32_vector;

#ifndef _MSC_VER
JM_GNUC_EXTENSION typedef signed long long int64;
JM_GNUC_EXTENSION typedef unsigned long long uint64;
#else
typedef signed __int64 int64;
typedef unsigned __int64 uint64;
#endif

#ifndef _MSC_VER
#define JM_INT64_CONSTANT(val) (JM_GNUC_EXTENSION (val##LL))
#else // _MSC_VER
#define JM_INT64_CONSTANT(val) (val##i64)
#endif // _MSC_VER
#ifndef _MSC_VER
#define JM_UINT64_CONSTANT(val) (JM_GNUC_EXTENSION (val##ULL))
#else // _MSC_VER
#define JM_UINT64_CONSTANT(val) (val##i64)
#endif // _MSC_VER

}

#endif