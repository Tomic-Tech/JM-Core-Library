#ifndef __JM_UTILS_H__
#define __JM_UTILS_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmtype.h>

JM_BEGIN_DECLS

#define JM_HIGH_BYTE(value) (uint8_t)(((uint16_t)(value) >> 8) & 0xFF)
#define JM_LOW_BYTE(value) (uint8_t)((uint16_t)(value) & 0xFF)
#define JM_HIGH_WORD(value) (uint16_t)(((uint32_t)(value) >> 16) & 0xFFF)
#define JM_LOW_WORD(value) (uint16_t)((uint32_t)(value) & 0xFFFF)

JM_END_DECLS

#endif