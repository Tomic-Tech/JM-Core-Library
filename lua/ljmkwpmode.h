#ifndef __LUA_JM_KWPMODE_H__
#define __LUA_JM_KWPMODE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmlib.h>

typedef struct _LuaJMKWPMode
{
    const char *name;
    JMKWPMode mode;
} LuaJMKWPMode;

extern LuaJMKWPMode lua_jm_kwp_mode[6];

#endif