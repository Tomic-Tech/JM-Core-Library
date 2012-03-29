#include <jm/jmlib.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_kwp1281_addr_init(lua_State *L) 
{
    uint8_t addr_code = (uint8_t)luaL_checkinteger(L, 1);

    if (jm_link_protocol_type() != JM_PRC_KWP1281)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp1281_addr_init(addr_code) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int _lua_jm_kwp1281_set_lines(lua_State *L) 
{
    int32_t com_line = luaL_checkinteger(L, 1);
    boolean_t l_line = lua_toboolean(L, 2);

    if (jm_link_protocol_type() != JM_PRC_KWP1281)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp1281_set_lines(com_line, l_line)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg _lua_jm_kwp1281_lib[] =
{
    {"addrInit", _lua_jm_kwp1281_addr_init},
    {"setLines", _lua_jm_kwp1281_set_lines},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmkwp1281(lua_State *L)
{
    luaL_newlib(L, _lua_jm_kwp1281_lib);
    return 1;
}
