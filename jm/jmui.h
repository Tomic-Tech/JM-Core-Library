#ifndef __JM_UI_H__
#define __JM_UI_H__

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


G_END_DECLS

#endif