#include <jm/jmcommboxfactory.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include <jm/jmkwp1281.h>

static int _lua_jm_kwp1281_addr_init(lua_State *L) {
    guint8 addr_code = (guint8)luaL_checkinteger(L, 1);
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;
    JMKWP1281 *kwp = (JMKWP1281*)jm_comm_get_protocol(comm);

    if (comm->prc_type != JM_PRC_KWP1281) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp1281_addr_init(kwp, addr_code) != JM_ERROR_SUCCESS) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int _lua_jm_kwp1281_set_lines(lua_State *L) {
    gint32 com_line = luaL_checkinteger(L, 1);
    gboolean l_line = lua_toboolean(L, 2);
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;
    JMKWP1281 *kwp = (JMKWP1281*)jm_comm_get_protocol(comm);

    if (comm->prc_type != JM_PRC_KWP1281) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_kwp1281_set_lines(kwp, com_line, l_line)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg _lua_jm_kwp1281_lib[] = {
    {"addrInit", _lua_jm_kwp1281_addr_init},
    {"setLines", _lua_jm_kwp1281_set_lines},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmkwp1281(lua_State *L) {
    luaL_newlib(L, _lua_jm_kwp1281_lib);
    return 1;
}
