#ifndef __JM_STRING_ARRAY_H__
#define __JM_STRING_ARRAY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmtype.h>

JM_BEGIN_DECLS

typedef struct _JMStringArray JMStringArray;
JMStringArray* jm_string_array_new(void);
void jm_string_array_free(JMStringArray *array);
void jm_string_array_append(JMStringArray *array, const char *str);
size_t jm_string_array_size(JMStringArray *array);
const char* jm_string_array_get(JMStringArray *array, size_t index);

JM_END_DECLS

#endif