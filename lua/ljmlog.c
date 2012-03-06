#include <jm/jmlog.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_log_write(lua_State *L)
{
    const gchar *text = luaL_checkstring(L, 1);
    jm_log_write(JM_LOG_DEBUG, "Lua Debug", text);
    return 0;
}

static int _lua_jm_log_write_hex(lua_State *L)
{
    size_t l;
    const gchar *text = luaL_checklstring(L, 1, &l);
    jm_log_write_hex(JM_LOG_DEBUG, "Lua Debug", text, l);
    return 0;
}

static const luaL_Reg _lua_jm_log_lib[] =
{
    {"write", _lua_jm_log_write},
    {"writeHex", _lua_jm_log_write_hex},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmlog(lua_State *L)
{
    luaL_newlib(L, _lua_jm_log_lib);
    return 1;
}