#ifndef __JM_TIMER_H__
#define __Jm_TIMER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmtype.h>

JM_BEGIN_DECLS

#define JM_TIMER_TO_MS(val) ((val) * 1000)
#define JM_TIMER_TO_SEC(val) ((val) * 1000000)

JM_END_DECLS

#endif