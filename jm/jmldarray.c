#include "jmldarray.h"
#include "jmdb.h"

static GMutex *_jm_ld_array_mutex = NULL;
static JMLDArray *_jm_ld_array = NULL;

#define _jm_ld_array_index(array, index) ((JMLiveData*)g_ptr_array_index(array->ld_array, index))
#define _jm_ld_array_size(array) (array->ld_array->len)


void jm_ld_array_init(void)
{
    _jm_ld_array_mutex = g_mutex_new();
}

void jm_ld_array_destroy(void)
{
    g_mutex_free(_jm_ld_array_mutex);
    if (_jm_ld_array != NULL)
        jm_ld_array_free(_jm_ld_array);
}

static void _jm_live_data_free(gpointer data)
{
    jm_live_data_free((JMLiveData*)data);
}

JMLDArray* jm_ld_array_new(void)
{
    JMLDArray *lda = (JMLDArray*)g_malloc(sizeof(JMLDArray));

    lda->ld_array = g_ptr_array_new_full(0, _jm_live_data_free);
    lda->current_enabled_index = -1;
    lda->show_index_array = g_ptr_array_new();
    lda->enabled_index_array = g_ptr_array_new();
    lda->show_position_array = g_hash_table_new(g_direct_hash, g_direct_equal);
    lda->enabled_size = 0;
    lda->show_size = 0;

    return lda;
}

void jm_ld_array_free(JMLDArray *self)
{
    size_t i;
    g_return_if_fail(self != NULL);

    g_ptr_array_free(self->ld_array, TRUE);
    g_ptr_array_free(self->show_index_array, TRUE);
    g_ptr_array_free(self->enabled_index_array, TRUE);
    g_hash_table_unref(self->show_position_array);
}

void jm_ld_array_append(JMLDArray *array, JMLiveData *ld)
{
    if (ld->enabled)
        array->enabled_size++;
    if (ld->enabled && ld->show)
        array->show_size++;
    g_ptr_array_add(array->ld_array, ld);
}

void jm_ld_array_load(void)
{
    g_mutex_lock(_jm_ld_array_mutex);
    if (_jm_ld_array != NULL)
        jm_ld_array_free(_jm_ld_array);

    _jm_ld_array = jm_db_get_live_data();

    g_mutex_unlock(_jm_ld_array_mutex);
}

gint32 jm_ld_array_next_show_index(void)
{
    gint32 ret = 0;
    size_t size;
    g_return_val_if_fail(_jm_ld_array != NULL, -1);
    g_mutex_lock(_jm_ld_array_mutex);

    if (_jm_ld_array->show_index_array->len == 0)
    {
        g_mutex_unlock(_jm_ld_array_mutex);
        return -1;
    }

    size = _jm_ld_array->show_index_array->len;
    ret = GPOINTER_TO_INT(g_ptr_array_index(_jm_ld_array->show_index_array, _jm_ld_array->current_enabled_index));
    _jm_ld_array->current_enabled_index++;
    if ((size_t)_jm_ld_array->current_enabled_index > (size - 1))
        _jm_ld_array->current_enabled_index = 0;

    g_mutex_unlock(_jm_ld_array_mutex);
    return ret;
}

gint32 jm_ld_array_get_enabled_index(gint32 index)
{
    gint32 ret;
    g_return_val_if_fail(_jm_ld_array != NULL, -1);

    g_mutex_lock(_jm_ld_array_mutex);

    ret = GPOINTER_TO_INT(g_ptr_array_index(_jm_ld_array->enabled_index_array, index));

    g_mutex_unlock(_jm_ld_array_mutex);

    return ret;
}

gint32 jm_ld_array_get_show_position(gint32 index)
{
    gint32 ret;
    g_return_val_if_fail(_jm_ld_array != NULL, -1);
    g_return_val_if_fail(_jm_ld_array->show_index_array->len > 0, -1);

    g_mutex_lock(_jm_ld_array_mutex);

    ret = GPOINTER_TO_INT(g_hash_table_lookup(_jm_ld_array->show_position_array, GINT_TO_POINTER(index)));

    g_mutex_unlock(_jm_ld_array_mutex);

    return ret;
}

gint32 jm_ld_array_get_show_index(gint32 index)
{
    gint32 ret;
    g_return_val_if_fail(_jm_ld_array != NULL, -1);
    g_return_val_if_fail(_jm_ld_array->show_index_array->len > 0, -1);

    g_mutex_lock(_jm_ld_array_mutex);

    ret = GPOINTER_TO_INT(g_ptr_array_index(_jm_ld_array->show_index_array, index));

    g_mutex_unlock(_jm_ld_array_mutex);

    return ret;
}

void jm_ld_array_generate_enabled_index(void)
{
    size_t i;
    size_t size;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);

    g_ptr_array_set_size(_jm_ld_array->enabled_index_array, 0);

    size = _jm_ld_array_size(_jm_ld_array);

    for (i = 0; i < size; i++)
    {
        JMLiveData *ld = _jm_ld_array_index(_jm_ld_array, i);
        if (ld->enabled)
            g_ptr_array_add(_jm_ld_array->enabled_index_array, GINT_TO_POINTER(i));
    }

    g_mutex_unlock(_jm_ld_array_mutex);
}

void jm_ld_array_generate_show_index(void)
{
    size_t i;
    size_t j = 0;
    size_t size;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);

    g_ptr_array_set_size(_jm_ld_array->show_index_array, 0);

    size = _jm_ld_array_size(_jm_ld_array);

    for (i = 0; i < size; i++)
    {
        JMLiveData *ld = _jm_ld_array_index(_jm_ld_array, i);
        if (ld->enabled && ld->show)
        {
            g_ptr_array_add(_jm_ld_array->show_index_array, GINT_TO_POINTER(i));
            g_hash_table_insert(_jm_ld_array->show_position_array, GINT_TO_POINTER(i), GINT_TO_POINTER(j++));
        }
    }

    _jm_ld_array->current_enabled_index = GPOINTER_TO_INT(g_ptr_array_index(_jm_ld_array->show_index_array, 0));
    g_mutex_unlock(_jm_ld_array_mutex);
}

size_t jm_ld_array_size(void)
{
    size_t size = 0;
    g_return_val_if_fail(_jm_ld_array != NULL, size);

    g_mutex_lock(_jm_ld_array_mutex);

    size = _jm_ld_array_size(_jm_ld_array);

    g_mutex_unlock(_jm_ld_array_mutex);

    return size;
}

size_t jm_ld_array_enabled_size(void)
{
    return _jm_ld_array->enabled_size;
}

size_t jm_ld_array_show_size(void)
{
    return _jm_ld_array->show_size;
}

void jm_ld_array_set_short_name(gint32 index, const gchar *short_name)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    ld->short_name = g_string_assign(ld->short_name, short_name);

    g_mutex_unlock(_jm_ld_array_mutex);
}

GLIB_VAR const gchar* jm_ld_array_get_short_name(gint32 index)
{
    JMLiveData *ld = NULL;
    gchar *short_name = "";
    g_return_val_if_fail(_jm_ld_array != NULL, short_name);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    short_name = ld->short_name->str;
    g_mutex_unlock(_jm_ld_array_mutex);

    return short_name;
}

void jm_ld_array_set_content(gint32 index, const gchar *content)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    ld->content = g_string_assign(ld->content, content);

    g_mutex_unlock(_jm_ld_array_mutex);
}

const gchar* jm_ld_array_get_content(gint32 index)
{
    JMLiveData *ld = NULL;
    gchar *content = "";
    g_return_val_if_fail(_jm_ld_array != NULL, content);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    content = ld->content->str;
    g_mutex_unlock(_jm_ld_array_mutex);

    return content;
}

void jm_ld_array_set_unit(gint32 index, const gchar *unit)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    ld->unit = g_string_assign(ld->unit, unit);

    g_mutex_unlock(_jm_ld_array_mutex);
}

const gchar* jm_ld_array_get_unit(gint32 index)
{
    JMLiveData *ld = NULL;
    gchar *unit = "";
    g_return_val_if_fail(_jm_ld_array != NULL, unit);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    unit = ld->unit->str;
    g_mutex_unlock(_jm_ld_array_mutex);

    return unit;
}

void jm_ld_array_set_default_value(gint32 index, const gchar *value)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    ld->default_value = g_string_assign(ld->default_value, value);

    g_mutex_unlock(_jm_ld_array_mutex);
}

const gchar* jm_ld_array_get_default_value(gint32 index)
{
    JMLiveData *ld = NULL;
    gchar *default_value = "";
    g_return_val_if_fail(_jm_ld_array != NULL, default_value);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    default_value = ld->default_value->str;
    g_mutex_unlock(_jm_ld_array_mutex);

    return default_value;
}

void jm_ld_array_set_value(gint32 index, const gchar *value)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    ld->value = g_string_assign(ld->value, value);

    g_mutex_unlock(_jm_ld_array_mutex);
}

const gchar* jm_ld_array_get_value(gint32 index)
{
    JMLiveData *ld = NULL;
    gchar *value = "";
    g_return_val_if_fail(_jm_ld_array != NULL, value);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    value = ld->value->str;
    g_mutex_unlock(_jm_ld_array_mutex);

    return value;
}

void jm_ld_array_set_command_id(gint32 index, gint32 id)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    ld->command_id = id;

    g_mutex_unlock(_jm_ld_array_mutex);
}

gint32 jm_ld_array_get_command_id(gint32 index)
{
    JMLiveData *ld = NULL;
    gint32 id = -1;
    g_return_val_if_fail(_jm_ld_array != NULL, -1);

    g_mutex_lock(_jm_ld_array_mutex);

    ld = _jm_ld_array_index(_jm_ld_array, index);
    id = ld->command_id;
    g_mutex_unlock(_jm_ld_array_mutex);

    return id;
}

void jm_ld_array_set_enabled(gint32 index, gboolean enabled)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);

    if (!ld->enabled)
    {
        if (enabled)
        {
            _jm_ld_array->enabled_size++;
            if (ld->show)
            {
                _jm_ld_array->show_size++;
            }
        }
    }
    else
    {
        if (!enabled)
        {
            _jm_ld_array->enabled_size--;
            if (ld->show)
            {
                _jm_ld_array->show_size--;
            }
        }
    }

    ld->enabled = enabled;

    g_mutex_unlock(_jm_ld_array_mutex);
}

gboolean jm_ld_array_get_enabled(gint32 index)
{
    JMLiveData *ld = NULL;
    gboolean enabled = FALSE;
    g_return_val_if_fail(_jm_ld_array != NULL, FALSE);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);
    enabled = ld->enabled;

    g_mutex_unlock(_jm_ld_array_mutex);
    return enabled;
}

void jm_ld_array_set_show(gint32 index, gboolean show)
{
    JMLiveData *ld = NULL;
    g_return_if_fail(_jm_ld_array != NULL);

    g_mutex_lock(_jm_ld_array_mutex);
    ld = _jm_ld_array_index(_jm_ld_array, index);

    if (!ld->show)
    {
        if (ld->enabled && show)
        {
            _jm_ld_array->show_size++;
        }
    }
    else
    {
        if (ld->enabled && !show)
        {
            _jm_ld_array->show_size--;
        }
    }

    ld->show = show;

    g_mutex_unlock(_jm_ld_array_mutex);
}

gboolean jm_ld_array_get_show(gint32 index)
{
    JMLiveData *ld = NULL;
    gboolean show = FALSE;

    g_return_val_if_fail(_jm_ld_array != NULL, FALSE);

    g_mutex_lock(_jm_ld_array_mutex);

    ld = _jm_ld_array_index(_jm_ld_array, index);
    show = ld->show;

    g_mutex_unlock(_jm_ld_array_mutex);

    return show;
}

