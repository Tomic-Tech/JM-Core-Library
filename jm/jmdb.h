#ifndef __JM_DB_H__
#define __JM_DB_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmldarray.h>

G_BEGIN_DECLS

void jm_db_init(void);
void jm_db_destroy(void);
gboolean jm_db_open(const gchar* file_path, const gchar* password);
gboolean jm_db_close(void);
gboolean jm_db_is_open(void);
void jm_db_set_tc_catalog(const gchar *catalog);
void jm_db_set_ld_catalog(const gchar *catalog);
void jm_db_set_cmd_catalog(const gchar *catalog);
gchar *jm_db_get_text(const gchar *name);
gchar *jm_db_get_trouble_code(const gchar *code);
GByteArray *jm_db_get_command(const gchar *name);
GByteArray *jm_db_get_command_id(gint32 id);
JMLDArray *jm_db_get_live_data(void);
void jm_db_load_live_data(void);

G_END_DECLS

#endif