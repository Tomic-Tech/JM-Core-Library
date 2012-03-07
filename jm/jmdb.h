#ifndef __JM_DB_H__
#define __JM_DB_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmldarray.h>

G_BEGIN_DECLS

GLIB_VAR void jm_db_init(void);
GLIB_VAR void jm_db_destroy(void);
GLIB_VAR gboolean jm_db_open(const gchar* file_path, const gchar* password);
GLIB_VAR gboolean jm_db_close(void);
GLIB_VAR gboolean jm_db_is_open(void);
GLIB_VAR void jm_db_set_tc_catalog(const gchar *catalog);
GLIB_VAR void jm_db_set_ld_catalog(const gchar *catalog);
GLIB_VAR void jm_db_set_cmd_catalog(const gchar *catalog);
GLIB_VAR gchar *jm_db_get_text(const gchar *name);
GLIB_VAR gchar *jm_db_get_trouble_code(const gchar *code);
GLIB_VAR GByteArray *jm_db_get_command(const gchar *name);
GLIB_VAR GByteArray *jm_db_get_command_by_id(gint32 id);
GLIB_VAR JMLDArray *jm_db_get_live_data(void);
G_END_DECLS

#endif