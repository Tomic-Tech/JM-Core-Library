#include <jm/jmlib.h>

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

    gint32 target_id = luaL_checkinteger(L, 1);
    const gchar*baud = luaL_checklstring(L, 2, &baud_length);
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
        g_ascii_strncasecmp(baud, lua_jm_canbus_baud[baud_i].name, baud_length) != 0)
    {
        baud_i++;
    }
    while (lua_jm_canbus_id_mode[id_mode_i].name != NULL &&
        g_ascii_strncasecmp(id_mode, lua_jm_canbus_id_mode[id_mode_i].name, id_mode_length) != 0)
    {
        id_mode_i++;
    }
    while (lua_jm_canbus_filter_mask[mask_i].name != NULL &&
        g_ascii_strncasecmp(mask, lua_jm_canbus_filter_mask[mask_i].name, mask_length) != 0)
    {
        mask_i++;
    }
    while (lua_jm_canbus_frame_type[frame_i].name != NULL &&
        g_ascii_strncasecmp(frame, lua_jm_canbus_frame_type[frame_i].name, frame_length) != 0)
    {
        frame_i++;
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
    guint32 high = luaL_checkinteger(L, 1);
    guint32 low = luaL_checkinteger(L, 2);

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
    gint32 id[16] = {0};
    

    if (jm_link_protocol_type() != JM_PRC_ISO15765)
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    while (TRUE)
    {
        /* Add key we're interested in to the stack */
        lua_pushinteger(L, i + 1);
        /* Have Lua functions look up the keyed value */
        lua_gettable(L, -2);
        if (!lua_isnil(L, -1) && lua_isnumber(L, -1) && i != 16)
        { /* Check if the last table key or none number */
            id[i] = (gint32)lua_tonumber(L, -1);
            lua_pop(L, 1);
            i++;
        }
        else
        {
            lua_pop(L, 1);
            break;
        }
    }
    if (i == 0)
        lua_pushboolean(L, 0);
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

static const luaL_Reg _lua_jm_iso15765_lib[] =
{
    {"setOptions", _lua_jm_iso15765_set_options},
    {"setLines", _lua_jm_iso15765_set_lines},
    {"setFilter", _lua_jm_iso15765_set_filter},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmiso15765(lua_State *L)
{
    luaL_newlib(L, _lua_jm_iso15765_lib);
    return 1;
}