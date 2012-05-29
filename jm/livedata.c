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

gint32 live_data_cmd_id(LiveData *obj)
{
    g_return_val_if_fail(obj != NULL, 0);
    return obj->cmdID;
}

