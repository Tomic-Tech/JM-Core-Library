#include "livedata.h"

LiveData * live_data_new(const gchar *shortName,
    const gchar *content,
    const gchar *unit,
    const gchar *defaultValue,
    const gchar *value,
    gint32 cmdID)
{
    LiveData *obj = (LiveData*)g_malloc(sizeof(LiveData));
    obj->shortName = g_string_new(shortName);
    obj->content = g_string_new(content);
    obj->unit = g_string_new(unit);
    obj->defaultValue = g_string_new(defaultValue);
    obj->value = g_string_new(value);
    obj->cmdID = cmdID;
    obj->enabled = TRUE;
    obj->showed = FALSE;
    obj->index = -1;
    return obj;
}

void live_data_dispose(LiveData *obj)
{
    g_return_if_fail(obj != NULL);
    g_string_free(obj->shortName, TRUE);
    g_string_free(obj->content, TRUE);
    g_string_free(obj->unit, TRUE);
    g_string_free(obj->defaultValue, TRUE);
    g_string_free(obj->value, TRUE);
    g_free(obj);
}

const gchar* live_data_short_name(LiveData *obj)
{
  g_return_val_if_fail(obj != NULL, NULL);
  return obj->shortName->str;
}

const gchar* live_data_content(LiveData *obj)
{
    g_return_val_if_fail(obj != NULL, NULL);
    return obj->content->str;
}

const gchar* live_data_unit(LiveData *obj)
{
    g_return_val_if_fail(obj != NULL, NULL);
    return obj->unit->str;
}

const gchar* live_data_default_value(LiveData *obj)
{
    g_return_val_if_fail(obj != NULL, NULL);
    return obj->defaultValue->str;
}

const gchar* live_data_value(LiveData *obj)
{
    g_return_val_if_fail(obj != NULL, NULL);
    return obj->value->str;
}

void live_data_set_value(LiveData *obj, const gchar *value)
{
  g_return_if_fail(obj != NULL);
  g_string_truncate(obj->value, 0);
  g_string_append(obj->value, value);
}

gint32 live_data_cmd_id(LiveData *obj)
{
    g_return_val_if_fail(obj != NULL, 0);
    return obj->cmdID;
}

gboolean live_data_enabled(LiveData *obj)
{
  g_return_val_if_fail(obj != NULL, FALSE);
  return obj->enabled;
}

void live_data_set_enabled(LiveData *obj, gboolean enabled)
{
  g_return_if_fail(obj != NULL);
  obj->enabled = enabled;
}

gboolean live_data_showed(LiveData *obj)
{
  g_return_val_if_fail(obj != NULL, FALSE);
  return obj->showed;
}

void live_data_set_showed(LiveData *obj, gboolean showed)
{
  g_return_if_fail(obj != NULL);
  obj->showed = showed;
}

gint32 live_data_index(LiveData *obj)
{
  g_return_val_if_fail(obj != NULL, -1);
  return obj->index;
}

void live_data_set_index(LiveData *obj, gint32 index)
{
  g_return_if_fail(obj != NULL);
  obj->index = index;
}

