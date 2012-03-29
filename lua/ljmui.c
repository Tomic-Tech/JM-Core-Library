#include <jm/jmlib.h>
#include <stdlib.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int _lua_jm_ui_main_show(lua_State *L)
{
    jm_ui_main_show();
    return 0;
}

static int _lua_jm_ui_msg_box_btn_clr(lua_State *L)
{
    jm_ui_msg_box_btn_clr();
    return 0;
}

static int _lua_jm_ui_msg_box_add_btn(lua_State *L)
{
    const char* text = (const char*)luaL_checkstring(L, 1);
    jm_ui_msg_box_add_btn(text);
    return 0;
}

static int _lua_jm_ui_msg_box_set_msg(lua_State *L)
{
    const char* text = (const char*)luaL_checkstring(L, 1);
    jm_ui_msg_box_set_msg(text);
    return 0;
}

static int _lua_jm_ui_msg_box_set_title(lua_State *L)
{
    const char* text = (const char*)luaL_checkstring(L, 1);
    jm_ui_msg_box_set_title(text);
    return 0;
}

static int _lua_jm_ui_msg_box_show(lua_State *L)
{
    jm_ui_msg_box_show();
    return 0;
}

static int _lua_jm_ui_msg_box_hide(lua_State *L)
{
    jm_ui_msg_box_hide();
    return 0;
}

static int _lua_jm_ui_list_box_btn_clr(lua_State *L)
{
    jm_ui_list_box_btn_clr();
    return 0;
}

static int _lua_jm_ui_list_box_add_btn(lua_State *L)
{
    const char *text = (const char*)luaL_checkstring(L, 1);
    jm_ui_list_box_add_btn(text);
    return 0;
}

static int _lua_jm_ui_list_box_add_item(lua_State *L)
{
    const char *caption = (const char*)luaL_checkstring(L, 1);
    const char *item = (const char*)luaL_checkstring(L, 2);
    jm_ui_list_box_add_item(caption, item);
    return 0;
}

static int _lua_jm_ui_list_box_item_clr(lua_State *L)
{
    jm_ui_list_box_item_clr();
    return 0;
}

static int _lua_jm_ui_list_box_show(lua_State *L)
{
    jm_ui_list_box_show();
    return 0;
}

static int _lua_jm_ui_list_box_hide(lua_State *L)
{
    jm_ui_list_box_hide();
    return 0;
}

static int _lua_jm_ui_menu_item_clr(lua_State *L)
{
    jm_ui_menu_item_clr();
    return 0;
}

static int _lua_jm_ui_menu_add_item(lua_State *L)
{
    const char *text = (const char*)luaL_checkstring(L, 1);
    jm_ui_menu_add_item(text);
    return 0;
}

static int _lua_jm_ui_menu_show(lua_State *L)
{
    jm_ui_menu_show();
    return 0;
}

static int _lua_jm_ui_tc_item_clr(lua_State *L)
{
    jm_ui_tc_item_clr();
    return 0;
}

static int _lua_jm_ui_tc_add_item(lua_State *L)
{
    const char *code = (const char*)luaL_checkstring(L, 1);
    const char *text = (const char*)luaL_checkstring(L, 2);
    jm_ui_tc_add_item(code, text);
    return 0;
}

static int _lua_jm_ui_tc_add_btn(lua_State *L)
{
    const char *text = (const char*)luaL_checkstring(L, 1);
    jm_ui_tc_add_btn(text);
    return 0;
}

static int _lua_jm_ui_tc_btn_clr(lua_State *L)
{
    jm_ui_tc_btn_clr();
    return 0;
}

static int _lua_jm_ui_tc_show(lua_State *L)
{
    jm_ui_tc_show();
    return 0;
}

static int _lua_jm_ui_ld_prepare_show(lua_State *L)
{
    jm_ui_ld_prepare_show();
    return 0;
}

static int _lua_jm_ui_ld_show(lua_State *L)
{
    jm_ui_ld_show();
    return 0;
}

static int _lua_jm_ui_ld_btn_clr(lua_State *L)
{
    jm_ui_ld_btn_clr();
    return 0;
}

static int _lua_jm_ui_ld_add_btn(lua_State *L)
{
    const char *text = (const char*)luaL_checkstring(L, 1);
    jm_ui_ld_add_btn(text);
    return 0;
}

static int _lua_jm_ui_ld_set_value(lua_State *L)
{
    int32_t index = luaL_checkinteger(L, 1);
    const char *value = (const char*)luaL_checkstring(L, 2);
    jm_ui_ld_set_value(index - 1, value);
    //g_usleep(1000);
    return 0;
}

static int _lua_jm_ui_get_btn_clicked(lua_State *L)
{
    boolean_t is_blocked = lua_toboolean(L, 1);
    char *ret = jm_ui_get_btn_clicked(is_blocked);
    if (ret == NULL)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushstring(L, ret);
    }
    free(ret);
    return 1;
}

static int _lua_jm_ui_get_menu_selected(lua_State *L)
{
    char *ret = jm_ui_get_menu_selected();
    if (ret == NULL)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushstring(L, ret);
    }
    free(ret);
    return 1;
}

static const luaL_Reg _lua_jm_ui_lib[] =
{
    {"mainShow", _lua_jm_ui_main_show},
    {"msgBoxBtnClr", _lua_jm_ui_msg_box_btn_clr},
    {"msgBoxAddBtn", _lua_jm_ui_msg_box_add_btn},
    {"msgBoxSetMsg", _lua_jm_ui_msg_box_set_msg},
    {"msgBoxSetTitle", _lua_jm_ui_msg_box_set_title},
    {"msgBoxShow", _lua_jm_ui_msg_box_show},
    {"msgBoxHide", _lua_jm_ui_msg_box_hide},
    {"listBoxBtnClr", _lua_jm_ui_list_box_btn_clr},
    {"listBoxAddBtn", _lua_jm_ui_list_box_add_btn},
    {"listBoxAddItem", _lua_jm_ui_list_box_add_item},
    {"listBoxItemClr", _lua_jm_ui_list_box_item_clr},
    {"listBoxShow", _lua_jm_ui_list_box_show},
    {"listBoxHide", _lua_jm_ui_list_box_hide},
    {"menuItemClr", _lua_jm_ui_menu_item_clr},
    {"menuAddItem", _lua_jm_ui_menu_add_item},
    {"menuShow", _lua_jm_ui_menu_show},
    {"tcItemClr", _lua_jm_ui_tc_item_clr},
    {"tcAddItem", _lua_jm_ui_tc_add_item},
    {"tcAddBtn", _lua_jm_ui_tc_add_btn},
    {"tcBtnClr", _lua_jm_ui_tc_btn_clr},
    {"tcShow", _lua_jm_ui_tc_show},
    {"ldPrepareShow", _lua_jm_ui_ld_prepare_show},
    {"ldShow", _lua_jm_ui_ld_show},
    {"ldBtnClr", _lua_jm_ui_ld_btn_clr},
    {"ldAddBtn", _lua_jm_ui_ld_add_btn},
    {"ldSetValue", _lua_jm_ui_ld_set_value},
    {"btnClked", _lua_jm_ui_get_btn_clicked},
    {"menuSelected", _lua_jm_ui_get_menu_selected},
    {NULL, NULL}
};

LUALIB_API int luaopen_jmui(lua_State *L)
{
    luaL_newlib(L, _lua_jm_ui_lib);
    return 1;
}

