#ifndef __JM_STRING_ARRAY_H__
#define __JM_STRING_ARRAY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _JMStringArray JMStringArray;

JMStringArray* jm_string_array_new(void);
void jm_string_array_free(JMStringArray *array);
void jm_string_array_append(JMStringArray *array, const gchar *str);
size_t jm_string_array_size(JMStringArray *array);
const gchar* jm_string_array_get(JMStringArray *array, size_t index);

G_END_DECLS

#endif