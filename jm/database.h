#ifndef __JM_DATABASE_H__
#define __JM_DATABASE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/livedatavector.h>
#include <sqlite3/sqlite3.h>

G_BEGIN_DECLS

void database_init(const char *path);
void database_dispose(void);
gboolean database_get_text(const char *name, const char *cls, wchar_t *text);
gboolean database_get_trouble_code(const char *code, const char *cls, wchar_t *content, wchar_t *description);
gboolean database_live_data_prepare(const char *cls);
gboolean database_live_data_next(wchar_t *shortName, wchar_t *content, wchar_t *unit, wchar_t *defaultValue, wchar_t *cmdName, wchar_t *cmdClass, wchar_t* description);
gboolean database_get_command(const char *name, const char *cls, uint8_t *buffer, size_t *count);
gboolean database_get_command_by_id(int32_t id, uint8_t *buffer, size_t *count);

G_END_DECLS

#endif /* __JM_DATABASE_H__ */
