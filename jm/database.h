#ifndef __JM_DATABASE_H__
#define __JM_DATABASE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/livedatavector.h>
#include <sqlite3/sqlite3.h>

G_BEGIN_DECLS

//typedef struct _Database Database;
//
//struct _Database {
//	GString *tcCatalog;
//	GString *ldCatalog;
//	GString *cmdCatalog;
//	sqlite3 *handle;
//	sqlite3_stmt *getTextStmt;
//	sqlite3_stmt *getTroubleCodeStmt;
//	sqlite3_stmt *getCommandStmt;
//	sqlite3_stmt *getCommandByIDStmt;
//	sqlite3_stmt *getLiveDataStmt;
//};
//
//Database * database_new(const gchar *filePath);
//void database_dispose(Database *obj);
//void database_set_tc_catalog(Database *obj, const gchar *catalog);
//void database_set_ld_catalog(Database *obj, const gchar *catalog);
//void database_set_cmd_catalog(Database *obj, const gchar *catalog);
//gboolean database_get_text(Database *obj, const gchar *name, gunichar2 *text);
//gboolean database_get_trouble_code(Database *obj, const gchar *code,
//		gunichar2 *text);
//gboolean database_live_data_prepare(Database *obj);
//gboolean database_live_data_next(Database *obj, gunichar2 *shortName,
//		gunichar2 *content, gunichar2 *unit, gunichar2 *defaultValue,
//		gunichar2 *value, gint32 *cmdID);
////gboolean database_get_live_data(Database *obj, LiveDataVector *vec);
//gboolean database_get_command(Database *obj, const gchar *name, guint8 *buffer,
//		size_t *count);
//gboolean database_get_command_by_id(Database *obj, gint32 id, guint8 *buffer,
//		size_t *count);

void database_init(const gchar *path);
void database_dispose(void);
gboolean database_get_text(const gchar *name, const gchar *cls, gunichar2 *text);
gboolean database_get_trouble_code(const gchar *code, const gchar *cls, gunichar2 *content, gunichar2 *description);
gboolean database_live_data_prepare(const gchar *cls);
gboolean database_live_data_next(gunichar2 *shortName, gunichar2 *content, gunichar2 *unit, gunichar2 *defaultValue, gint32 *cmdID, gunichar2* description);
gboolean database_get_command(const gchar *name, const gchar *cls, guint8 *buffer, size_t *count);
gboolean database_get_command_by_id(gint32 id, guint8 *buffer, size_t *count);

G_END_DECLS

#endif /* __JM_DATABASE_H__ */
