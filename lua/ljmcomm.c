#include <jm/jmcommboxfactory.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_comm_send_one_frame(lua_State *L) {
    size_t l = 0;
    const guint8 *data = (const guint8*)luaL_checklstring(L, 1, &l);
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;
    size_t ret = jm_comm_send_one_frame(comm, data, l);

    lua_pushinteger(L, ret);
    return 1;
}

static int _lua_jm_comm_send_frames(lua_State *L) {
    size_t l = 0;
    const guint8 *data = (const guint8*)luaL_checklstring(L, 1, &l);
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;
    size_t ret = jm_comm_send_frames(comm, data, l);

    lua_pushinteger(L, ret);
    return 1;
}

static int _lua_jm_comm_read_one_frame(lua_State *L) {
    static guint8 data[0xFFFF];
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;

    size_t ret = jm_comm_read_one_frame(comm, data);

    if (ret == 0) {
        lua_pushnil(L);
    } else {
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
    static guint8 data[0xFFFFFFF];
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;

    size_t ret = jm_comm_read_frames(comm, data);

    if (ret == 0) {
        lua_pushnil(L);
    } else {
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

static int _lua_jm_comm_send_and_recv(lua_State *L) {
    size_t l = 0;
    const guint8 *send = (const guint8*)luaL_checklstring(L, 1, &l);
    static guint8 recv[0xFFFFFFF];
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;

    size_t ret = jm_comm_send_and_recv(comm, send, l, recv);

    if (ret == 0) {
        lua_pushnil(L);
    } else {
        luaL_Buffer b;
        size_t i;
        luaL_buffinit(L, &b);

        for (i = 0; i < ret; i++) {
            luaL_addchar(&b, recv[i]);
        }
        luaL_pushresult(&b);

    }

    return 1;
}

static int _lua_jm_comm_start_keep_link(lua_State *L) {
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;
    gboolean run = lua_toboolean(L, 1);

    if (jm_comm_start_keep_link(comm, run) == JM_ERROR_SUCCESS) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

static int _lua_jm_comm_set_keep_link(lua_State *L) {
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;
    size_t l = 0;
    const guint8 *data = (const guint8*)luaL_checklstring(L, 1, &l);

    if (jm_comm_set_keep_link(comm, data, l) == JM_ERROR_SUCCESS) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

static int _lua_jm_comm_set_timeout(lua_State *L) {
    JMComm *comm = jm_commbox_factory_create_commbox()->comm;

    gint32 tx_b2b = luaL_checkinteger(L, 1);
    gint32 tx_f2f = luaL_checkinteger(L, 2);
    gint32 rx_b2b = luaL_checkinteger(L, 3);
    gint32 rx_f2f = luaL_checkinteger(L, 4);
    gint32 total = luaL_checkinteger(L, 5);

    if (jm_comm_set_timeout(comm, tx_b2b, rx_b2b, tx_f2f, rx_f2f, total) == JM_ERROR_SUCCESS) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

static const luaL_Reg _lua_jm_comm_lib[] = {
    {"sendOneFrame", _lua_jm_comm_send_one_frame},
    {"sendFrames", _lua_jm_comm_send_frames},
    {"readOneFrame", _lua_jm_comm_read_one_frame},
    {"readFrames", _lua_jm_comm_read_frames},
    {"sendAndRecv", _lua_jm_comm_send_and_recv},
    {"startKeepLink", _lua_jm_comm_start_keep_link},
    {"setKeepLink", _lua_jm_comm_set_keep_link},
    {"setTimeout", _lua_jm_comm_set_timeout},
};

LUALIB_API int luaopen_jmcomm(lua_State *L) {
    luaL_newlib(L, _lua_jm_comm_lib);
    return 1;
}
