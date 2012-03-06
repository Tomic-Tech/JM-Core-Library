/*
** $Id: lualib.h,v 1.43 2011/12/08 12:11:37 roberto Exp $
** Lua standard libraries
** See Copyright Notice in lua.h
*/


#ifndef lualib_h
#define lualib_h

#include "lua.h"



LUAMOD_API int (luaopen_base) (lua_State *L);

#define LUA_COLIBNAME	"coroutine"
LUAMOD_API int (luaopen_coroutine) (lua_State *L);

#define LUA_TABLIBNAME	"table"
LUAMOD_API int (luaopen_table) (lua_State *L);

#define LUA_IOLIBNAME	"io"
LUAMOD_API int (luaopen_io) (lua_State *L);

#define LUA_OSLIBNAME	"os"
LUAMOD_API int (luaopen_os) (lua_State *L);

#define LUA_STRLIBNAME	"string"
LUAMOD_API int (luaopen_string) (lua_State *L);

#define LUA_BITLIBNAME	"bit32"
LUAMOD_API int (luaopen_bit32) (lua_State *L);

#define LUA_MATHLIBNAME	"math"
LUAMOD_API int (luaopen_math) (lua_State *L);

#define LUA_DBLIBNAME	"debug"
LUAMOD_API int (luaopen_debug) (lua_State *L);

#define LUA_LOADLIBNAME	"package"
LUAMOD_API int (luaopen_package) (lua_State *L);

#define LUA_JMDBNAME "jmdb"
LUAMOD_API int (luaopen_jmdb) (lua_State *L);

#define LUA_JMBOXNAME "jmbox"
LUAMOD_API int (luaopen_jmbox) (lua_State *L);

#define LUA_JMCOMMNAME "jmcomm"
LUAMOD_API int (luaopen_jmcomm) (lua_State *L);

#define LUA_JMISO14230 "jmiso14230"
LUAMOD_API int (luaopen_jmiso14230) (lua_State *L);

#define LUA_JMISO15765NAME "jmiso15765"
LUAMOD_API int (luaopen_jmiso15765) (lua_State *L);

#define LUA_JMKWP1281NAME "jmkwp1281"
LUAMOD_API int (luaopen_jmkwp1281) (lua_State *L);

#define LUA_JMMIKUNINAME "jmmikuni"
LUAMOD_API int (luaopen_jmmikuni) (lua_State *L);

#define LUA_JMUINAME "jmui"
LUAMOD_API int (luaopen_jmui) (lua_State *L);

#define LUA_JMLOG "jmlog"
LUAMOD_API int (luaopen_jmlog) (lua_State *L);

/* open all previous libraries */
LUALIB_API void (luaL_openlibs) (lua_State *L);



#if !defined(lua_assert)
#define lua_assert(x)	((void)0)
#endif


#endif
