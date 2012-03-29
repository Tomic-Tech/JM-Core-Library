#include <jm/jmlib.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_comm_send_one_frame(lua_State *L)
{
    size_t l = 0;
    const uint8_t *data = (const uint8_t*)luaL_checklstring(L, 1, &l);
    size_t ret = jm_link_send_one_frame(data, l);

    lua_pushinteger(L, ret);
    return 1;
}

static int _lua_jm_comm_send_frames(lua_State *L)
{
    size_t l = 0;
    const uint8_t *data = (const uint8_t*)luaL_checklstring(L, 1, &l);
    size_t ret = jm_link_send_frames(data, l);

    lua_pushinteger(L, ret);
    return 1;
}

static int _lua_jm_comm_read_one_frame(lua_State *L) {
    static uint8_t data[0xFFFF];

    size_t ret = jm_link_read_one_frame(data, 0xFFFF);

    if (ret == 0)
    {
        lua_pushnil(L);
    }
    else
    {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);

        for (i = 0; i < ret; i++) {
            luaL_addchar(&b, data[i]);
        }
        luaL_pushresult(&b);
    }

    return 1;

}

static int _lua_jm_comm_read_frames(lua_State *L) {
    static uint8_t data[0xFFFFFFF];

    size_t ret = jm_link_read_frames(data, 0xFFFFFFF);

    if (ret == 0)
    {
        lua_pushnil(L);
    }
    else
    {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);

        for (i = 0; i < ret; i++)
        {
            luaL_addchar(&b, data[i]);
        }
        luaL_pushresult(&b);
    }

    return 1;
}

static int _lua_jm_comm_send_and_recv(lua_State *L)
{
    size_t l = 0;
    const uint8_t *send = (const uint8_t*)luaL_checklstring(L, 1, &l);
    static uint8_t recv[0xFFFFFFF];

    size_t ret = jm_link_send_and_recv(send, l, recv, 0xFFFFFFF);

    if (ret == 0)
    {
        lua_pushnil(L);
    }
    else
    {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);

        for (i = 0; i < ret; i++)
        {
            luaL_addchar(&b, recv[i]);
        }
        luaL_pushresult(&b);

    }

    return 1;
}

static int _lua_jm_comm_start_keep_link(lua_State *L)
{

    boolean_t run = lua_toboolean(L, 1);

    if (jm_link_start_keep_link(run) == JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }

    return 1;
}

static int _lua_jm_comm_set_keep_link(lua_State *L) {
    size_t l = 0;
    const uint8_t *data = (const uint8_t*)luaL_checklstring(L, 1, &l);

    if (jm_link_set_keep_link(data, l) == JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }

    return 1;
}

static int _lua_jm_comm_set_timeout(lua_State *L) {
    int64_t tx_b2b = luaL_checkinteger(L, 1);
    int64_t tx_f2f = luaL_checkinteger(L, 2);
    int64_t rx_b2b = luaL_checkinteger(L, 3);
    int64_t rx_f2f = luaL_checkinteger(L, 4);
    int64_t total = luaL_checkinteger(L, 5);

    if (jm_link_set_timeout(tx_b2b, rx_b2b, tx_f2f, rx_f2f, total) == JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }

    return 1;
}

static const luaL_Reg _lua_jm_comm_lib[] =
{
    {"sendOneFrame", _lua_jm_comm_send_one_frame},
    {"sendFrames", _lua_jm_comm_send_frames},
    {"readOneFrame", _lua_jm_comm_read_one_frame},
    {"readFrames", _lua_jm_comm_read_frames},
    {"sendAndRecv", _lua_jm_comm_send_and_recv},
    {"startKeepLink", _lua_jm_comm_start_keep_link},
    {"setKeepLink", _lua_jm_comm_set_keep_link},
    {"setTimeout", _lua_jm_comm_set_timeout},
};

LUALIB_API int luaopen_jmcomm(lua_State *L)
{
    luaL_newlib(L, _lua_jm_comm_lib);
    return 1;
}
