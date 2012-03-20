#ifndef __LUA_JM_CANBUS_H__
#define __LUA_JM_CANBUS_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmlib.h>

typedef struct _LuaJMCanbusBaud {
    const char *name;
    JMCanbusBaud baud;
} LuaJMCanbusBaud;

typedef struct _LuaJMCanbusIDMode {
    const char *name;
    JMCanbusIDMode mode;
} LuaJMCanbusIDMode;

typedef struct _LuaJMCanbusFrameType {
    const char *name;
    JMCanbusFrameType frame;
} LuaJMCanbusFrameType;

typedef struct _LuaJMCanbusFilterMask {
    const char *name;
    JMCanbusFilterMask mask;
} LuaJMCanbusFilterMask;

extern LuaJMCanbusBaud lua_jm_canbus_baud[16];
extern LuaJMCanbusIDMode lua_jm_canbus_id_mode[3];
extern LuaJMCanbusFrameType lua_jm_canbus_frame_type[3];
extern LuaJMCanbusFilterMask lua_jm_canbus_filter_mask[3];

#endif