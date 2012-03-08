#include "jmui.h"
#include "jmlog.h"

static GQueue *_jm_ui_msg_queue;
static GMutex *_jm_ui_mutex;
static GString *_jm_ui_btn_clicked = NULL;
static GMutex *_jm_ui_btn_mutex = NULL;
static GCond *_jm_ui_btn_cond = NULL;

static GString *_jm_ui_menu_sel = NULL;
static GMutex *_jm_ui_menu_mutex = NULL;

void jm_ui_init(void)
{
    _jm_ui_msg_queue = g_queue_new();
    _jm_ui_mutex = g_mutex_new();
    _jm_ui_btn_mutex = g_mutex_new();
    _jm_ui_btn_cond = g_cond_new();
    _jm_ui_menu_mutex = g_mutex_new();
}

void jm_ui_msg_free(JMUIMessage *obj)
{
    g_return_if_fail(obj != NULL);
    if (obj->message != NULL)
        g_free(obj->message);
    g_free(obj);
}

static void _jm_ui_msg_free(gpointer data, gpointer user_data)
{
    jm_ui_msg_free((JMUIMessage*)data);
}

void jm_ui_destroy(void)
{
    g_mutex_free(_jm_ui_mutex);
    g_mutex_free(_jm_ui_btn_mutex);
    g_cond_free(_jm_ui_btn_cond);
    g_mutex_free(_jm_ui_menu_mutex);
    if (_jm_ui_btn_clicked != NULL)
        g_string_free(_jm_ui_btn_clicked, TRUE);

    if (_jm_ui_menu_sel != NULL)
        g_string_free(_jm_ui_menu_sel, TRUE);

    g_queue_foreach(_jm_ui_msg_queue, _jm_ui_msg_free, NULL);
    g_queue_free(_jm_ui_msg_queue);
}

JMUIMessage* jm_ui_pop_msg(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    if (!g_queue_is_empty(_jm_ui_msg_queue))
    {
        msg = (JMUIMessage*)g_queue_pop_head(_jm_ui_msg_queue);
    }
    g_mutex_unlock(_jm_ui_mutex);
    return msg;
}

size_t jm_ui_msg_count(void)
{
    size_t ret;
    g_mutex_lock(_jm_ui_mutex);
    ret = _jm_ui_msg_queue->length;
    g_mutex_unlock(_jm_ui_mutex);
    return ret;
}

void jm_ui_main_show(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->message = NULL;
    msg->type = JM_UI_MAIN_SHOW;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_msg_box_btn_clr(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MSG_BOX_BTN_CLR;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_msg_box_add_btn(const gchar *text)
{
    GString *str = g_string_new(text);
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MSG_BOX_ADD_BTN;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_msg_box_set_msg(const gchar *text)
{
    GString *str = g_string_new(text);
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MSG_BOX_SET_MSG;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_msg_box_set_title(const gchar *text)
{
    GString *str = g_string_new(text);
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MSG_BOX_SET_TITLE;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_msg_box_show(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MSG_BOX_SHOW;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_msg_box_hide(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MSG_BOX_HIDE;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_list_box_btn_clr(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LIST_BOX_BTN_CLR;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_list_box_add_btn(const gchar *text)
{
    GString *str = g_string_new(text);
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LIST_BOX_ADD_BTN;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_list_box_add_item(const gchar *caption, const gchar *item)
{
    GString *str = g_string_new("");
    JMUIMessage *msg = NULL;
    g_string_append_printf(str, "%s|%s", caption, item);

    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LIST_BOX_ADD_ITEM;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_list_box_item_clr(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LIST_BOX_ITEM_CLR;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_list_box_show(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LIST_BOX_SHOW;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_list_box_hide(void) {
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LIST_BOX_HIDE;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_menu_item_clr(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MENU_ITEM_CLR;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_menu_add_item(const gchar *text)
{
    GString *str = g_string_new(text);
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MENU_ADD_ITEM;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_menu_show(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_MENU_SHOW;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_tc_item_clr(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_TC_ITEM_CLR;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_tc_add_item(const gchar *code, const gchar *text)
{
    GString *str = g_string_new("");
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    g_string_append_printf(str, "%s|%s", code, text);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_TC_ADD_ITEM;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_tc_add_btn(const gchar *text)
{
    GString *str = g_string_new(text);
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_TC_ADD_BTN;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_tc_btn_clr(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_TC_BTN_CLR;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_tc_show(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_TC_SHOW;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_ld_prepare_show(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LD_PREPARE_SHOW;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_ld_show(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LD_SHOW;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_ld_btn_clr(void)
{
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LD_BTN_CLR;
    msg->message = NULL;
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_ld_add_btn(const gchar *text)
{
    GString *str = g_string_new(text);
    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LD_ADD_BTN;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

void jm_ui_ld_set_value(gint32 index, const gchar *value)
{
    GString *str = g_string_new("");

    JMUIMessage *msg = NULL;
    g_mutex_lock(_jm_ui_mutex);
    g_string_append_printf(str, "%d|%s", index, value);
    msg = g_malloc(sizeof(JMUIMessage));
    msg->type = JM_UI_LD_SET_VALUE;
    msg->message = g_string_free(str, FALSE);
    g_queue_push_tail(_jm_ui_msg_queue, msg);
    g_mutex_unlock(_jm_ui_mutex);
}

gchar* jm_ui_get_btn_clicked(gboolean is_blocked)
{
    gchar *ret = NULL;
    g_mutex_lock(_jm_ui_btn_mutex);
    if (is_blocked)
    {
        while (_jm_ui_btn_clicked == NULL)
            g_cond_wait(_jm_ui_btn_cond, _jm_ui_btn_mutex);
        ret = g_string_free(_jm_ui_btn_clicked, FALSE);
        _jm_ui_btn_clicked = NULL;
    }
    else
    {
        GTimeVal time_val;
        g_get_current_time(&time_val);
        g_time_val_add(&time_val, 1000);
        g_cond_timed_wait(_jm_ui_btn_cond, _jm_ui_btn_mutex, &time_val);
        if (_jm_ui_btn_clicked == NULL)
        {
        }
        else
        {
            ret = g_string_free(_jm_ui_btn_clicked, FALSE);
            _jm_ui_btn_clicked = NULL;
        }
    }
    g_mutex_unlock(_jm_ui_btn_mutex);
    return ret;
}

void jm_ui_set_btn_clicked(const gchar *text)
{
    g_mutex_lock(_jm_ui_btn_mutex);
    if (_jm_ui_btn_clicked == NULL)
    {
        _jm_ui_btn_clicked = g_string_new(text);
    }
    else
    {
        _jm_ui_btn_clicked = g_string_assign(_jm_ui_btn_clicked, text);
    }
    g_cond_signal(_jm_ui_btn_cond);
    g_mutex_unlock(_jm_ui_btn_mutex);
}

gchar* jm_ui_get_menu_selected(void)
{
    g_mutex_lock(_jm_ui_menu_mutex);
    if (_jm_ui_menu_sel == NULL)
    {
        g_mutex_unlock(_jm_ui_menu_mutex);
        return NULL;
    }
    else
    {
        gchar *ret = g_string_free(_jm_ui_menu_sel, FALSE);
        _jm_ui_menu_sel = NULL;
        g_mutex_unlock(_jm_ui_menu_mutex);
        return ret;
    }
    return NULL;
}

void jm_ui_set_menu_selected(const gchar *text)
{
    g_mutex_lock(_jm_ui_menu_mutex);

    if (_jm_ui_menu_sel == NULL)
    {
        _jm_ui_menu_sel = g_string_new(text);
    }
    else
    {
        _jm_ui_menu_sel = g_string_assign(_jm_ui_menu_sel, text);
    }
    g_mutex_unlock(_jm_ui_menu_mutex);
}

