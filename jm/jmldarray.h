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
};

#define jm_ld_array_append(array, ld) g_ptr_array_add(array->ld_array, ld)

void jm_ld_array_init(void);
void jm_ld_array_destroy(void);
JMLDArray* jm_ld_array_new(void);
void jm_ld_array_free(JMLDArray *self);
void jm_ld_array_load(void);
GLIB_VAR gint32 jm_ld_array_next_show_index(void);
GLIB_VAR void jm_ld_array_generate_show_index(void);
GLIB_VAR size_t jm_ld_array_size(void);
GLIB_VAR void jm_ld_array_set_short_name(gint32 index, const gchar *short_name);
GLIB_VAR const gchar* jm_ld_array_get_short_name(gint32 index);
GLIB_VAR void jm_ld_array_set_content(gint32 index, const gchar *content);
GLIB_VAR const gchar* jm_ld_array_get_content(gint32 index);
GLIB_VAR void jm_ld_array_set_unit(gint32 index, const gchar *unit);
GLIB_VAR const gchar* jm_ld_array_get_unit(gint32 index);
GLIB_VAR void jm_ld_array_set_default_value(gint32 index, const gchar *value);
GLIB_VAR const gchar* jm_ld_array_get_default_value(gint32 index);
GLIB_VAR void jm_ld_array_set_value(gint32 index, const gchar *value);
GLIB_VAR const gchar* jm_ld_array_get_value(gint32 index);
GLIB_VAR void jm_ld_array_set_command_id(gint32 index, gint32 id);
GLIB_VAR gint32 jm_ld_array_get_command_id(gint32 index);
GLIB_VAR void jm_ld_array_set_enabled(gint32 index, gboolean enabled);
GLIB_VAR gboolean jm_ld_array_get_enabled(gint32 index);
GLIB_VAR void jm_ld_array_set_show(gint32 index, gboolean show);
GLIB_VAR gboolean jm_ld_array_get_show(gint32 index);

G_END_DECLS

#endif