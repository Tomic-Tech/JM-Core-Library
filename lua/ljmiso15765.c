#include <jm/jmlib.h>
#include <string.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "ljmcanbus.h"

static int _lua_jm_iso15765_set_options(lua_State *L)
{
    size_t baud_length = 0;
    size_t id_mode_length = 0;
    size_t mask_length = 0;
    size_t frame_length = 0;

    int32_t target_id = luaL_checkinteger(L, 1);
    const char*baud = luaL_checklstring(L, 2, &baud_length);
    const char *id_mode = luaL_checklstring(L, 3, &id_mode_length);
    const char *mask = luaL_checklstring(L, 4, &mask_length);
    const char *frame = luaL_checklstring(L, 5, &frame_length);

    size_t baud_i = 0;
    size_t id_mode_i = 0;
    size_t mask_i = 0;
    size_t frame_i = 0;

    if (jm_link_protocol_type() != JM_PRC_ISO15765)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    while (lua_jm_canbus_baud[baud_i].name != NULL &&
        strncmp(baud, lua_jm_canbus_baud[baud_i].name, baud_length) != 0)
    {
        ++baud_i;
    }
    while (lua_jm_canbus_id_mode[id_mode_i].name != NULL &&
        strncmp(id_mode, lua_jm_canbus_id_mode[id_mode_i].name, id_mode_length) != 0)
    {
        ++id_mode_i;
    }
    while (lua_jm_canbus_filter_mask[mask_i].name != NULL &&
        strncmp(mask, lua_jm_canbus_filter_mask[mask_i].name, mask_length) != 0)
    {
        ++mask_i;
    }
    while (lua_jm_canbus_frame_type[frame_i].name != NULL &&
        strncmp(frame, lua_jm_canbus_frame_type[frame_i].name, frame_length) != 0)
    {
        ++frame_i;
    }
    if (lua_jm_canbus_baud[baud_i].name == NULL ||
        lua_jm_canbus_id_mode[id_mode_i].name == NULL ||
        lua_jm_canbus_filter_mask[mask_i].name == NULL ||
        lua_jm_canbus_frame_type[frame_i].name == NULL)
    {
        //        lua_pushstring(L, "invalid canbus options");
        //        lua_error(L);
        lua_pushboolean(L, 0);
    }
    else
    {
        if (jm_canbus_set_options(target_id,
            lua_jm_canbus_baud[baud_i].baud,
            lua_jm_canbus_id_mode[id_mode_i].mode,
            lua_jm_canbus_filter_mask[mask_i].mask,
            lua_jm_canbus_frame_type[frame_i].frame) != JM_ERROR_SUCCESS)
        {
            //            lua_pushstring(L, "JMLib commbox setting iso15765 options fail!");
            //            lua_error(L);
            lua_pushboolean(L, 0);
        }
        else
        {
            lua_pushboolean(L, 1);
        }
    }
    return 1;
}

static int _lua_jm_iso15765_set_lines(lua_State *L)
{
    int32_t high = luaL_checkinteger(L, 1);
    int32_t low = luaL_checkinteger(L, 2);

    if (jm_link_protocol_type() != JM_PRC_ISO15765)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_canbus_set_lines(high, low) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
    }
    else
    {
        lua_pushboolean(L, 1);
    }
    return 1;
}

static int _lua_jm_iso15765_set_filter(lua_State *L)
{
    size_t i = 0;
    int32_t id[16] = {0};
	int32_t l;
    
	if (jm_link_protocol_type() != JM_PRC_ISO15765)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	l = lua_gettop(L);
	lua_pushnil(L);
    while (lua_next(L, l))
    {
		id[i++] = luaL_checknumber(L, -1);
		lua_pop(L, 1);
    }
    if (i == 0)
	{
        lua_pushboolean(L, 0);
		return 1;
	}
    if (jm_canbus_set_filter(id, i) != JM_ERROR_SUCCESS)
    {
        lua_pushboolean(L, 0);
    }
    else
    {
        lua_pushboolean(L, 1);
    }
    return 1;
}

static int _lua_jm_iso15765_init(lua_State *L)
{
	if (jm_canbus_init() != JM_ERROR_SUCCESS)
	{
		lua_pushboolean(L, 0);
	}
	else
	{
		lua_pushboolean(L, 1);
	}
	return 1;
}

static const luaL_Reg _lua_jm_iso15765_lib[] =
{
    {"setOptions", _lua_jm_iso15765_set_options},
    {"setLines", _lua_jm_iso15765_set_lines},
    {"setFilter", _lua_jm_iso15765_set_filter},
	{"init", _lua_jm_iso15765_init},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmiso15765(lua_State *L)
{
    luaL_newlib(L, _lua_jm_iso15765_lib);
    return 1;
}