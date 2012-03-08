#include "jmlivedata.h"

JMLiveData* jm_live_data_new(const gchar *short_name, 
    const gchar* content, 
    const gchar *unit, 
    const gchar *default_value, 
    gint32 command_id,
    gboolean showed)
{
    JMLiveData *ld = (JMLiveData*)g_malloc(sizeof(JMLiveData));

    ld->short_name = g_string_new(short_name);

    ld->content = g_string_new(content);

    ld->unit = g_string_new(unit);

    ld->default_value = g_string_new(default_value);

    ld->value = g_string_new("");

    ld->command_id = command_id;
    ld->enabled = TRUE;
    ld->show = showed;
    return ld;
}

void jm_live_data_free(JMLiveData *self)
{
    g_return_if_fail(self != NULL);

    g_string_free(self->short_name, TRUE);
    g_string_free(self->content, TRUE);
    g_string_free(self->unit, TRUE);
    g_string_free(self->default_value, TRUE);
    g_string_free(self->value, TRUE);
    g_free(self);
}

