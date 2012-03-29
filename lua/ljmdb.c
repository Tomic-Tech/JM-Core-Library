#include <jm/jmlib.h>
#include <stdlib.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_db_get_text(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    char *text = jm_db_get_text(name);
    if (text == NULL)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushstring(L, text);
        free(text);
    }
    return 1;
}

static int _lua_jm_db_set_tc_catalog(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    jm_db_set_tc_catalog(name);
    return 0;
}

static int _lua_jm_db_set_ld_catalog(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    jm_db_set_ld_catalog(name);
    return 0;
}

static int _lua_jm_db_set_cmd_catalog(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    jm_db_set_cmd_catalog(name);
    return 0;
}

static int _lua_jm_db_get_trouble_code(lua_State *L)
{
    const char *code = luaL_checkstring(L, 1);
    char *text = jm_db_get_trouble_code(code);
    lua_pushstring(L, text);
    free(text);
    return 1;
}

static int _lua_jm_db_get_command(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    JMByteArray *cmd = jm_db_get_command(name);
    if (cmd == NULL)
    {
        lua_pushnil(L);
    }
    else
    {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);
        for (i = 0; i < jm_byte_array_size(cmd); i++)
        {
            luaL_addchar(&b, jm_byte_array_data(cmd)[i]);
        }
        luaL_pushresult(&b);
        jm_byte_array_free(cmd);
    }
    return 1;
}

static int _lua_jm_db_get_command_id(lua_State *L)
{
    int id = lua_tointeger(L, 1);
    JMByteArray *cmd = jm_db_get_command_by_id(id);
    if (cmd == NULL)
    {
        lua_pushnil(L);
    } 
    else 
    {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);
        for (i = 0; i < jm_byte_array_size(cmd); i++)
        {
            luaL_addchar(&b, jm_byte_array_data(cmd)[i]);
        }
        luaL_pushresult(&b);
        jm_byte_array_free(cmd);
    }
    return 1;
}

static int _lua_jm_db_get_live_data(lua_State *L)
{
    size_t i;
    size_t size;
    boolean_t showed = lua_toboolean(L, 1);

    jm_ld_array_update_global_array(showed);

    lua_newtable(L); /* creates root table for LDArray */
    
    size = jm_ld_array_size();
    for (i = 0; i < size; i++)
    {
        lua_pushinteger(L, i + 1); /* C zero begin, lua one begin */
        lua_newtable(L); /* sub table */
        lua_pushstring(L, "shortName");
        lua_pushstring(L, jm_ld_array_get_short_name(i));
        lua_settable(L, -3);
        lua_pushstring(L, "enable");
        lua_pushboolean(L, jm_ld_array_get_enabled(i));
        lua_settable(L, -3);
        lua_pushstring(L, "commandID");
        lua_pushinteger(L, jm_ld_array_get_command_id(i));
        lua_settable(L, -3); /* now sub table is -1 key is -2, and main table is -3 */
        lua_settable(L, -3); /* set sub table to main table */
    }

    return 1;
}

static int _lua_jm_db_ld_deploy(lua_State *L)
{
    jm_ld_array_deploy_showed_index();

    return 0;
}

static int _lua_jm_db_ld_next_showed_index(lua_State *L)
{
    lua_pushinteger(L, jm_ld_array_next_showed_index() + 1);

    return 1;
}

static int _lua_jm_db_ld_set_enabled(lua_State *L)
{
    int32_t index = luaL_checkinteger(L, 1);
    boolean_t enabled = lua_toboolean(L, 2);

    jm_ld_array_set_enabled(index - 1, enabled);
    return 0;
}

static const luaL_Reg _lua_jm_db_lib[] =
{
    {"getText", _lua_jm_db_get_text},
    {"setTCCatalog", _lua_jm_db_set_tc_catalog},
    {"setLDCatalog", _lua_jm_db_set_ld_catalog},
    {"setCmdCatalog", _lua_jm_db_set_cmd_catalog},
    {"getTroubleCode", _lua_jm_db_get_trouble_code},
    {"getCommand", _lua_jm_db_get_command},
    {"getCommandId", _lua_jm_db_get_command_id},
    {"getLiveData", _lua_jm_db_get_live_data},
    {"ldDeploy", _lua_jm_db_ld_deploy},
    {"ldNextShowedIndex", _lua_jm_db_ld_next_showed_index},
    {"ldSetEnabled", _lua_jm_db_ld_set_enabled},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmdb(lua_State *L)
{
    luaL_newlib(L, _lua_jm_db_lib);
    return 1;
}
