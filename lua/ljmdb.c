#include <jm/jmdb.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_db_get_text(lua_State *L) {
    const gchar *name = luaL_checkstring(L, 1);
    gchar *text = jm_db_get_text(name);
    if (text == NULL) {
        lua_pushnil(L);
    } else {
        lua_pushstring(L, text);
        g_free(text);
    }
    return 1;
}

static int _lua_jm_db_set_tc_catalog(lua_State *L) {
    const gchar *name = luaL_checkstring(L, 1);
    jm_db_set_tc_catalog(name);
    return 0;
}

static int _lua_jm_db_set_ld_catalog(lua_State *L) {
    const gchar *name = luaL_checkstring(L, 1);
    jm_db_set_ld_catalog(name);
    return 0;
}

static int _lua_jm_db_set_cmd_catalog(lua_State *L) {
    const gchar *name = luaL_checkstring(L, 1);
    jm_db_set_cmd_catalog(name);
    return 0;
}

static int _lua_jm_db_get_trouble_code(lua_State *L) {
    const gchar *code = luaL_checkstring(L, 1);
    gchar *text = jm_db_get_trouble_code(code);
    lua_pushstring(L, text);
    g_free(text);
    return 1;
}

static int _lua_jm_db_get_command(lua_State *L) {
    const gchar *name = luaL_checkstring(L, 1);
    GByteArray *cmd = jm_db_get_command(name);
    if (cmd == NULL) {
        lua_pushnil(L);
    } else {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);
        for (i = 0; i < cmd->len; i++) {
            luaL_addchar(&b, cmd->data[i]);
        }
        luaL_pushresult(&b);
        g_byte_array_free(cmd, TRUE);
    }
    return 1;
}

static int _lua_jm_db_get_command_id(lua_State *L) {
    int id = lua_tointeger(L, 1);
    GByteArray *cmd = jm_db_get_command_id(id);
    if (cmd == NULL) {
        lua_pushnil(L);
    } else {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);
        for (i = 0; i < cmd->len; i++) {
            luaL_addchar(&b, cmd->data[i]);
        }
        luaL_pushresult(&b);
        g_byte_array_free(cmd, TRUE);
    }
    return 1;
}

static int _lua_jm_db_get_live_data(lua_State *L) {
    size_t i;
    JMLDArray *arr = NULL;

    jm_db_load_live_data();
    arr = jm_db_get_live_data();

    g_return_val_if_fail(arr != NULL, 0);

    lua_newtable(L); /* creates root table for LDArray */
    
    for (i = 0; i < jm_ld_array_size(arr); i++) {
        JMLiveData *ld = jm_ld_array_index(arr, i);
        lua_pushinteger(L, i + 1); /* C zero begin, lua one begin */
        lua_newtable(L); /* sub table */
        lua_pushstring(L, "shortName");
        lua_pushstring(L, ld->short_name);
        lua_settable(L, -3);
        lua_pushstring(L, "enable");
        lua_pushboolean(L, ld->enabled);
        lua_settable(L, -3);
        lua_pushstring(L, "commandID");
        lua_pushinteger(L, ld->command_id);
        lua_settable(L, -3); /* now sub table is -1 key is -2, and main table is -3 */
        lua_settable(L, -3); /* set sub table to main table */
    }

    lua_newtable(L); /* creates root table fore enable LDArray */

    return 1;
}

static int _lua_jm_db_ld_generate(lua_State *L) {
    JMLDArray *arr = jm_db_get_live_data();

    jm_ld_array_generate_enabled_index(arr);

    return 0;
}

static int _lua_jm_db_ld_next_index(lua_State *L) {
    JMLDArray *arr = jm_db_get_live_data();

    lua_pushinteger(L, jm_ld_array_next_enabled_index(arr));

    return 1;
}

static int _lua_jm_db_ld_set_enabled(lua_State *L) {
    JMLDArray *arr = jm_db_get_live_data();
    gint32 index = luaL_checkinteger(L, 1);
    gboolean enabled = lua_toboolean(L, 2);

    jm_ld_array_index(arr, index)->enabled = enabled;
    return 0;
}

static const luaL_Reg _lua_jm_db_lib[] = {
    {"getText", _lua_jm_db_get_text},
    {"setTCCatalog", _lua_jm_db_set_tc_catalog},
    {"setLDCatalog", _lua_jm_db_set_ld_catalog},
    {"setCmdCatalog", _lua_jm_db_set_cmd_catalog},
    {"getTroubleCode", _lua_jm_db_get_trouble_code},
    {"getCommand", _lua_jm_db_get_command},
    {"getCommandId", _lua_jm_db_get_command_id},
    {"getLiveData", _lua_jm_db_get_live_data},
    {"ldGenerate", _lua_jm_db_ld_generate},
    {"ldNextIndex", _lua_jm_db_ld_next_index},
    {"ldSetEnabled", _lua_jm_db_ld_set_enabled},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmdb(lua_State *L) {
    luaL_newlib(L, _lua_jm_db_lib);
    return 1;
}
