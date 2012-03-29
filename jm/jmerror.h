#ifndef __JM_ERROR_H__
#define __JM_ERROR_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmtype.h>

JM_BEGIN_DECLS

#define JM_ERROR_SUCCESS 0
#define JM_ERROR_GENERIC -1
#define JM_ERROR_COMMBOX_OPEN_FAIL -2
#define JM_ERROR_COMMBOX_TRY_AGAIN -3

JM_END_DECLS

#endif