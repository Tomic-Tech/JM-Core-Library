#ifndef __JM_UI_H__
#define __JM_UI_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMUIMsgType JMUIMsgType;

enum _JMUIMsgType
{
    JM_UI_MAIN_SHOW,
    JM_UI_MSG_BOX_BTN_CLR,
    JM_UI_MSG_BOX_ADD_BTN,
    JM_UI_MSG_BOX_SET_MSG,
    JM_UI_MSG_BOX_SET_TITLE,
    JM_UI_MSG_BOX_SHOW,
    JM_UI_MSG_BOX_HIDE,
    JM_UI_LIST_BOX_BTN_CLR,
    JM_UI_LIST_BOX_ADD_BTN,
    JM_UI_LIST_BOX_ITEM_CLR,
    JM_UI_LIST_BOX_ADD_ITEM,
    JM_UI_LIST_BOX_SHOW,
    JM_UI_LIST_BOX_HIDE,
    JM_UI_MENU_ITEM_CLR,
    JM_UI_MENU_ADD_ITEM,
    JM_UI_MENU_SHOW,
    JM_UI_TC_ITEM_CLR,
    JM_UI_TC_ADD_ITEM,
    JM_UI_TC_ADD_BTN,
    JM_UI_TC_BTN_CLR,
    JM_UI_TC_SHOW,
    JM_UI_LD_PREPARE_SHOW,
    JM_UI_LD_SHOW,
    JM_UI_LD_BTN_CLR,
    JM_UI_LD_ADD_BTN,
    JM_UI_LD_SET_VALUE,
	JM_UI_UNKNOW
};

typedef struct _JMUIMessage
{
    JMUIMsgType type;
    gchar *message;
} JMUIMessage;

GLIB_VAR void jm_ui_init(void);
GLIB_VAR void jm_ui_destroy(void);

GLIB_VAR void jm_ui_msg_free(JMUIMessage *obj);
GLIB_VAR void jm_ui_main_show(void);
GLIB_VAR void jm_ui_msg_box_btn_clr(void);
GLIB_VAR void jm_ui_msg_box_add_btn(const gchar *text);
GLIB_VAR void jm_ui_msg_box_set_msg(const gchar *text);
GLIB_VAR void jm_ui_msg_box_set_title(const gchar *text);
GLIB_VAR void jm_ui_msg_box_show(void);
GLIB_VAR void jm_ui_msg_box_hide(void);
GLIB_VAR void jm_ui_list_box_btn_clr(void);
GLIB_VAR void jm_ui_list_box_add_btn(const gchar *text);
GLIB_VAR void jm_ui_list_box_add_item(const gchar *caption, const gchar *item);
GLIB_VAR void jm_ui_list_box_item_clr(void);
GLIB_VAR void jm_ui_list_box_show(void);
GLIB_VAR void jm_ui_list_box_hide(void);
GLIB_VAR void jm_ui_menu_item_clr(void);
GLIB_VAR void jm_ui_menu_add_item(const gchar *text);
GLIB_VAR void jm_ui_menu_show(void);
GLIB_VAR void jm_ui_tc_item_clr(void);
GLIB_VAR void jm_ui_tc_add_item(const gchar *code, const gchar *text);
GLIB_VAR void jm_ui_tc_add_btn(const gchar *text);
GLIB_VAR void jm_ui_tc_btn_clr(void);
GLIB_VAR void jm_ui_tc_show(void);
GLIB_VAR void jm_ui_ld_prepare_show(void);
GLIB_VAR void jm_ui_ld_show(void);
GLIB_VAR void jm_ui_ld_btn_clr(void);
GLIB_VAR void jm_ui_ld_add_btn(const gchar *text);
GLIB_VAR void jm_ui_ld_set_value(gint32 index, const gchar *value);
GLIB_VAR gchar* jm_ui_get_btn_clicked(gboolean is_blocked);
GLIB_VAR void jm_ui_set_btn_clicked(const gchar *text);
GLIB_VAR gchar* jm_ui_get_menu_selected(void);
GLIB_VAR void jm_ui_set_menu_selected(const gchar *text);
GLIB_VAR JMUIMessage* jm_ui_pop_msg(void);

G_END_DECLS

#endif