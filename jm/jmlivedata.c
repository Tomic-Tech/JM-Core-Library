#include "jmlivedata.h"

JMLiveData* jm_live_data_new(const gchar *short_name, 
    const gchar* content, 
    const gchar *unit, 
    const gchar *default_value, 
    gint32 command_id, 
    gboolean enabled) {
        JMLiveData *ld = (JMLiveData*)g_malloc(sizeof(JMLiveData));
        
        GString *str = g_string_new(short_name);
        ld->short_name = g_string_free(str, FALSE);

        str = g_string_new(content);
        ld->content = g_string_free(str, FALSE);

        str = g_string_new(unit);
        ld->unit = g_string_free(str, FALSE);

        str = g_string_new(default_value);
        ld->default_value = g_string_free(str, FALSE);

        ld->command_id = command_id;
        ld->enabled = enabled;
}

void jm_live_data_free(JMLiveData *self) {
    g_return_if_fail(self != NULL);

    g_free(self->short_name);
    g_free(self->content);
    g_free(self->unit);
    g_free(self->default_value);
    g_free(self);
}