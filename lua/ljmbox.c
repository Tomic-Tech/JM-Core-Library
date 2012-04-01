#include <jm/jmlib.h>
#include <string.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static struct
{
    const char *name;
    JMProtocolType type;
} _lua_jm_protocol_type[] =
{
    {"J1850VPW", JM_PRC_J1850VPW},
    {"J1850PWM", JM_PRC_J1850PWM},
    {"ISO9141_2", JM_PRC_ISO9141_2},
    {"ISO14230", JM_PRC_ISO14230},
    {"ISO15765", JM_PRC_ISO15765},
    {"ALDL160", JM_PRC_ALDL160},
    {"ALDL8192", JM_PRC_ALDL8192},
    {"KWP1281", JM_PRC_KWP1281},
    {NULL, JM_PRC_UNKNOW} /* sentinel */
};

static struct
{
    const char *name;
    JMConnector connector;
} _lua_jm_connector[] =
{
    {"OBDII 16", JM_CN_OBDII_16},
    {"UNIVERSAL 3", JM_CN_UNIVERSAL_3},
    {"BENZ 38", JM_CN_BENZ_38},
    {"BMW 20", JM_CN_BMW_20},
    {"AUDI 4", JM_CN_AUDI_4},
    {"FIAT 3", JM_CN_FIAT_3},
    {"CITROEN 2", JM_CN_CITROEN_2},
    {"CHRYSLER 6", JM_CN_CHRYSLER_6},
    {"TOYOTA 17R", JM_CN_TOYOTA_17R},
    {"TOYOTA 17F", JM_CN_TOYOTA_17F},
    {"HONDA 3", JM_CN_HONDA_3},
    {"MITSUBISHI", JM_CN_MITSUBISHI},
    {"HYUNDAI", JM_CN_HYUNDAI},
    {"NISSAN", JM_CN_NISSAN},
    {"SUZUKI 3", JM_CN_SUZUKI_3},
    {"DAIHATSU 4", JM_CN_DAIHATSU_4},
    {"ISUZU 3", JM_CN_ISUZU_3},
    {"CANBUS 16", JM_CN_CANBUS_16},
    {"GM 12", JM_CN_GM_12},
    {"KIA 20", JM_CN_KIA_20},
    {"NISSAN 14", JM_CN_NISSAN_14},
    {NULL, JM_CN_UNKNOW} /* sentinel */
};

//static int _lua_jm_commbox_open(lua_State *L) {
//    JMCommbox *box = jm_commbox_factory_create_commbox();
//    if (jm_commbox_open(box) == JM_ERROR_SUCCESS) {
//        lua_pushboolean(L, 1);
//    } else {
//        lua_pushboolean(L, 0);
//    }
//    return 1;
//}
//
//static int _lua_jm_commbox_close(lua_State *L) {
//    JMCommbox *box = jm_commbox_factory_create_commbox();
//    if (jm_commbox_close(box) == JM_ERROR_SUCCESS) {
//        lua_pushboolean(L, 1);
//    } else {
//        lua_pushboolean(L, 0);
//    }
//
//    return 1;
//}

static int _lua_jm_commbox_configure(lua_State *L)
{
    size_t i = 0;
    size_t l = 0;
    const char* prc = luaL_checklstring(L, 1, &l);
    while (_lua_jm_protocol_type[i].name != NULL &&
        strncmp(prc, _lua_jm_protocol_type[i].name, l) != 0)
    {
        ++i;
    }
    if (_lua_jm_protocol_type[i].name == NULL)
    {
        lua_pushboolean(L, 0);
    }
    else
    {
        if (jm_commbox_configure(_lua_jm_protocol_type[i].type) != JM_ERROR_SUCCESS)
        {
            lua_pushboolean(L, 0);
        }
    }
    lua_pushboolean(L, 1);
    return 1;
}

static int _lua_jm_commbox_set_connector(lua_State *L)
{
    size_t i = 0;
    size_t l = 0;

    const char *cn = luaL_checklstring(L, 1, &l);

    while (_lua_jm_connector[i].name != NULL &&
        strncmp(cn, _lua_jm_connector[i].name, l) != 0)
    {
        ++i;
    }

    if (_lua_jm_connector[i].name == NULL)
    {
        lua_pushboolean(L, 0);
    }
    else
    {
        if (jm_commbox_set_connector(_lua_jm_connector[i].connector) != JM_ERROR_SUCCESS)
        {
            lua_pushboolean(L, 0);
        }
    }
    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg _lua_jm_box_lib[] =
{
    {"configure", _lua_jm_commbox_configure},
    {"setConnector", _lua_jm_commbox_set_connector},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmbox(lua_State *L)
{
    luaL_newlib(L, _lua_jm_box_lib);
    return 1;
}
