#ifndef __JM_STRING_ARRAY_H__
#define __JM_STRING_ARRAY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _JMStringArray JMStringArray;

GLIB_VAR JMStringArray* jm_string_array_new(void);
GLIB_VAR void jm_string_array_free(JMStringArray *array);
GLIB_VAR void jm_string_array_append(JMStringArray *array, const gchar *str);
GLIB_VAR size_t jm_string_array_size(JMStringArray *array);
GLIB_VAR const gchar* jm_string_array_get(JMStringArray *array, size_t index);

G_END_DECLS

#endif