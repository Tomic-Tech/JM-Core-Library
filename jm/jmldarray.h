#ifndef __JM_LD_ARRAY_H__
#define __JM_LD_ARRAY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmlivedata.h>

G_BEGIN_DECLS

typedef struct _JMLDArray JMLDArray;

struct _JMLDArray {
    GPtrArray *ld_array;
    GPtrArray *enabled_index_array;
    gint32 current_enabled_index;
    GMutex *mutex;
};

#define jm_ld_array_index(array, index) ((JMLiveData*)g_ptr_array_index(array->ld_array, index))
#define jm_ld_array_append(array, ld) g_ptr_array_add(array->ld_array, ld)
#define jm_ld_array_size(array) (array->ld_array->len)

GLIB_VAR JMLDArray* jm_ld_array_new(void);
GLIB_VAR void jm_ld_array_free(JMLDArray *self);
GLIB_VAR gint32 jm_ld_array_next_enabled_index(JMLDArray *self);
GLIB_VAR void jm_ld_array_generate_enabled_index(JMLDArray *self);

G_END_DECLS

#endif