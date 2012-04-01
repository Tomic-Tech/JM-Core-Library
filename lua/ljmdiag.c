#include <jm/jmlib.h>
#include <stdlib.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_diag_calc_std_obd_dtc(lua_State *L)
{
	size_t length;
	const uint8_t *buff = (const uint8_t*)luaL_checklstring(L, 1, &length);
	int32_t pos = luaL_checknumber(L, 2);
	int32_t factor = luaL_checknumber(L, 3);
	int32_t offset = luaL_checknumber(L, 4);

	char *code = jm_diag_calc_std_obd_dtc(buff, length, pos, factor, offset);
	lua_pushstring(L, code);

	free(code);
	return 1;
}

static const luaL_Reg _lua_jm_diag_lib[] =
{
	{"calcStdObdDtc", _lua_jm_diag_calc_std_obd_dtc},
	{NULL, NULL}
};

LUALIB_API int luaopen_jmdiag(lua_State *L)
{
	luaL_newlib(L, _lua_jm_diag_lib);
	return 1;
}