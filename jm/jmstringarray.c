#include "jmstringarray.h"

struct _JMStringArray {
    GPtrArray *str_array;
};

static void _jm_string_array_free(gpointer data, gpointer user_data)
{
    g_free(data);
}

JMStringArray* jm_string_array_new(void)
{
    JMStringArray *obj = (JMStringArray*)g_malloc(sizeof(JMStringArray));
    obj->str_array = g_ptr_array_new();
    return obj;
}

void jm_string_array_free(JMStringArray *array)
{
    g_return_if_fail(array != NULL);
    g_ptr_array_foreach(array->str_array, _jm_string_array_free, NULL);
    g_ptr_array_free(array->str_array, TRUE);
    g_free(array);
}

void jm_string_array_append(JMStringArray *array, const gchar *str)
{
    glong length = g_utf8_strlen(str, -1);
    gchar *tmp = NULL;
    g_return_if_fail(array != NULL);
    tmp = (gchar*)g_malloc(length + 1);
    g_utf8_strncpy(tmp, str, length);
    tmp[length] = 0;
    g_ptr_array_add(array->str_array, tmp);
}

size_t jm_string_array_size(JMStringArray *array)
{
    g_return_val_if_fail(array != NULL, 0);
    return array->str_array->len;
}

const gchar* jm_string_array_get(JMStringArray *array, size_t index)
{
    g_return_val_if_fail(array != NULL, NULL);
    g_return_val_if_fail(jm_string_array_size(array) > index, NULL);
    return (gchar*)g_ptr_array_index(array->str_array, index);
}
