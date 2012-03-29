#include <jm/jmlib.h>
#include <string.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "ljmkwpmode.h"

static int _lua_jm_iso14230_set_options(lua_State *L)
{
    size_t msg_mode_length = 0;
    size_t link_mode_length = 0;
    size_t msg_mode_i = 0;
    size_t link_mode_i = 0;
    const char *msg_mode = luaL_checklstring(L, 1, &msg_mode_length);
    const char *link_mode = luaL_checklstring(L, 2, &link_mode_length);
    int32_t baud = luaL_checkinteger(L, 3);

    if (jm_link_protocol_type() != JM_PRC_ISO14230)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    while (lua_jm_kwp_mode[msg_mode_i].name != NULL &&
        strncmp(msg_mode, lua_jm_kwp_mode[msg_mode_i].name, msg_mode_length) != 0)
        msg_mode_i++;

    while (lua_jm_kwp_mode[link_mode_i].name != NULL &&
        strncmp(link_mode, lua_jm_kwp_mode[link_mode_i].name, link_mode_length) != 0)
        link_mode_i++;

    if (lua_jm_kwp_mode[msg_mode_i].name == NULL ||
        lua_jm_kwp_mode[link_mode_i].name == NULL)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp2000_set_options(lua_jm_kwp_mode[msg_mode_i].mode,
        lua_jm_kwp_mode[link_mode_i].mode,
        baud) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushboolean(L, 1);

    return 1;

}

static int _lua_jm_iso14230_set_address(lua_State *L)
{
    uint8_t target_address = luaL_checkinteger(L, 1);
    uint8_t source_address = luaL_checkinteger(L, 2);

    if (jm_link_protocol_type() != JM_PRC_ISO14230)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp2000_set_address(target_address, source_address) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushboolean(L, 1);
    return 1;

}

static int _lua_jm_iso14230_set_lines(lua_State *L)
{
    int32_t com_line = luaL_checkinteger(L, 1);
    boolean_t l_line = lua_toboolean(L, 2);

    if (jm_link_protocol_type() != JM_PRC_ISO14230)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp2000_set_lines(com_line, l_line) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int _lua_jm_iso14230_fast_init(lua_State *L)
{
    size_t data_length = 0;
    const uint8_t *data = (const uint8_t*)luaL_checklstring(L, 1, &data_length);

    if (jm_link_protocol_type() != JM_PRC_ISO14230)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp2000_fast_init(data, data_length) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int _lua_jm_iso14230_addr_init(lua_State *L)
{
    uint8_t addr_code = (uint8_t)luaL_checkinteger(L, 1);

    if (jm_link_protocol_type() != JM_PRC_ISO14230)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp2000_addr_init(addr_code) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg _lua_jm_iso14230_lib[] =
{
    {"setOptions", _lua_jm_iso14230_set_options},
    {"setAddress", _lua_jm_iso14230_set_address},
    {"setLines", _lua_jm_iso14230_set_lines},
    {"fastInit", _lua_jm_iso14230_fast_init},
    {"addrInit", _lua_jm_iso14230_addr_init},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmiso14230(lua_State *L)
{
    luaL_newlib(L, _lua_jm_iso14230_lib);
    return 1;
}
