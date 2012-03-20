#include <jm/jmlib.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_mikuni_init(lua_State *L)
{
    if (jm_link_protocol_type() != JM_PRC_MIKUNI)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_mikuni_init() != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg _lua_jm_mikuni_lib[] = 
{
    {"init", _lua_jm_mikuni_init},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmmikuni(lua_State *L) 
{
    luaL_newlib(L, _lua_jm_mikuni_lib);
    return 1;
}
