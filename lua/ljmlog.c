#include <jm/jmlib.h>
#include <openssl/des.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_log_write(lua_State *L)
{
    const char *text = luaL_checkstring(L, 1);
    jm_log_write("Lua Debug", text);
    return 0;
}

static int _lua_jm_log_write_hex(lua_State *L)
{
    size_t l;
    const char *text = luaL_checklstring(L, 1, &l);
    jm_log_write_hex("Lua Debug", (const uint8_t*)text, l);
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