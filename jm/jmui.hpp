#ifndef __JM_UI_HPP__
#define __JM_UI_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmui.h>

namespace JM
{
    inline void jm_ui_msg_free(gpointer data, gpointer user_data)
    {
        JMUIMessage *obj = (JMUIMessage*)data;
        g_return_if_fail(obj != NULL);
        if (obj->message != NULL)
            g_free(obj->message);
        g_free(obj);
    }

    class UserInterface
    {
    public:
        static UserInterface& inst()
        {
            static UserInterface instance;
            return instance;
        }

        void msgFree(JMUIMessage *obj)
        {
            g_return_if_fail(obj != NULL);
            if (obj->message != NULL)
                g_free(obj->message);
            g_free(obj);
        }

        JMUIMessage* popMsg()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            if (!g_queue_is_empty(_msgQueue))
            {
                msg = (JMUIMessage *)g_queue_pop_head(_msgQueue);
            }
            g_mutex_unlock(_mutex);
            return msg;
        }

        size_t msgCount()
        {
            size_t ret;
            g_mutex_lock(_mutex);
            ret = _msgQueue->length;
            g_mutex_unlock(_mutex);
            return ret;
        }

        void mainShow()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->message = NULL;
            msg->type = JM_UI_MAIN_SHOW;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void msgBoxBtnClr()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MSG_BOX_BTN_CLR;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void msgBoxAddBtn(const gchar *text)
        {
            GString *str = g_string_new(text);
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MSG_BOX_ADD_BTN;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void msgBoxSetMsg(const gchar *text)
        {
            GString *str = g_string_new(text);
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MSG_BOX_SET_MSG;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void msgBoxSetTitle(const gchar *text)
        {
            GString *str = g_string_new(text);
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MSG_BOX_SET_TITLE;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void msgBoxShow()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MSG_BOX_SHOW;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void msgBoxHide()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MSG_BOX_HIDE;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void listBoxBtnClr()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LIST_BOX_BTN_CLR;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void listBoxAddBtn(const gchar *text)
        {
            GString *str = g_string_new(text);
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LIST_BOX_ADD_BTN;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void listBoxAddItem(const gchar *caption, const gchar *item)
        {
            GString *str = g_string_new("");
            JMUIMessage *msg = NULL;
            g_string_append_printf(str, "%s|%s", caption, item);

            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LIST_BOX_ADD_ITEM;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void listBoxItemClr()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LIST_BOX_ITEM_CLR;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void listBoxShow()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LIST_BOX_SHOW;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void listBoxHide()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LIST_BOX_HIDE;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void menuItemClr()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MENU_ITEM_CLR;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void menuAddItem(const gchar *text)
        {
            GString *str = g_string_new(text);
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MENU_ADD_ITEM;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void menuShow()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_MENU_SHOW;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void tcItemClr()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_TC_ITEM_CLR;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void tcAddItem(const gchar *code, const gchar *text)
        {
            GString *str = g_string_new("");
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            g_string_append_printf(str, "%s|%s", code, text);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_TC_ADD_ITEM;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void tcAddBtn(const gchar *text)
        {
            GString *str = g_string_new(text);
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_TC_ADD_BTN;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void tcBtnClr()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_TC_BTN_CLR;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void tcShow()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_TC_SHOW;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void ldPrepareShow()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LD_PREPARE_SHOW;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void ldShow()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LD_SHOW;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void ldBtnClr()
        {
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LD_BTN_CLR;
            msg->message = NULL;
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void ldAddBtn(const gchar *text)
        {
            GString *str = g_string_new(text);
            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LD_ADD_BTN;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        void ldSetValue(gint32 index, const gchar *value)
        {
            GString *str = g_string_new("");

            JMUIMessage *msg = NULL;
            g_mutex_lock(_mutex);
            g_string_append_printf(str, "%d|%s", index, value);
            msg = (JMUIMessage*)g_malloc(sizeof(JMUIMessage));
            msg->type = JM_UI_LD_SET_VALUE;
            msg->message = g_string_free(str, FALSE);
            g_queue_push_tail(_msgQueue, msg);
            g_mutex_unlock(_mutex);
        }

        gchar* btnClicked(gboolean isBlocked)
        {
            gchar *ret = NULL;
            g_mutex_lock(_btnMutex);
            if (isBlocked)
            {
                while (_btnClicked == NULL)
                    g_cond_wait(_btnCond, _btnMutex);
                ret = g_string_free(_btnClicked, FALSE);
                _btnClicked = NULL;
            }
            else
            {
                GTimeVal time_val;
                g_get_current_time(&time_val);
                g_time_val_add(&time_val, 4000);
                g_cond_timed_wait(_btnCond, _btnMutex, &time_val);
                if (_btnClicked == NULL)
                {
                }
                else
                {
                    ret = g_string_free(_btnClicked, FALSE);
                    _btnClicked = NULL;
                }
            }
            g_mutex_unlock(_btnMutex);
            //g_usleep(3000);
            return ret;
        }

        void setBtnClicked(const gchar *text)
        {
            g_mutex_lock(_btnMutex);
            if (_btnClicked == NULL)
            {
                _btnClicked = g_string_new(text);
            }
            else
            {
                _btnClicked = g_string_assign(_btnClicked, text);
            }
            g_cond_signal(_btnCond);
            g_mutex_unlock(_btnMutex);
        }

        gchar* menuSelected()
        {
            g_mutex_lock(_menuMutex);
            if (_menuSel == NULL)
            {
                g_mutex_unlock(_menuMutex);
                return NULL;
            }
            else
            {
                gchar *ret = g_string_free(_menuSel, FALSE);
                _menuSel = NULL;
                g_mutex_unlock(_menuMutex);
                return ret;
            }
            return NULL;
        }

        void setMenuSelected(const gchar *text)
        {
            g_mutex_lock(_menuMutex);

            if (_menuSel == NULL)
            {
                _menuSel = g_string_new(text);
            }
            else
            {
                _menuSel = g_string_assign(_menuSel, text);
            }
            g_mutex_unlock(_menuMutex);
        }

    private:
        UserInterface()
            : _msgQueue(g_queue_new())
            , _mutex(g_mutex_new())
            , _btnClicked(NULL)
            , _btnMutex(g_mutex_new())
            , _btnCond(g_cond_new())
            , _menuSel(NULL)
            , _menuMutex(g_mutex_new())
        {
        }

        ~UserInterface()
        {
            g_mutex_free(_mutex);
            g_mutex_free(_btnMutex);
            g_cond_free(_btnCond);
            g_mutex_free(_menuMutex);

            if (_btnClicked != NULL)
                g_string_free(_btnClicked, TRUE);

            if (_menuSel != NULL)
                g_string_free(_menuSel, TRUE);

            g_queue_foreach(_msgQueue, jm_ui_msg_free, NULL);
            g_queue_free(_msgQueue);
        }
    private:
        GQueue *_msgQueue;
        GMutex *_mutex;
        GString *_btnClicked;
        GMutex *_btnMutex;
        GCond *_btnCond;
        GString *_menuSel;
        GMutex *_menuMutex;
    };
}

#endif