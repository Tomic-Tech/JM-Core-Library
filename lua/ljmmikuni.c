#include <jm/jmcommboxfactory.h>
#include <jm/jmmikuni.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_mikuni_init(lua_State *L) {
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;
    JMMikuni *m = (JMMikuni*)jm_comm_get_protocol(comm);

    if (comm->prc_type != JM_PRC_MIKUNI) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (jm_mikuni_init(m) != JM_ERROR_SUCCESS) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg _lua_jm_mikuni_lib[] = {
    {"init", _lua_jm_mikuni_init},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmmikuni(lua_State *L) {
    luaL_newlib(L, _lua_jm_mikuni_lib);
    return 1;
}
