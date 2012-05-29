#ifndef __JM_SYSDB_H__
#define __JM_SYSDB_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <sqlite3/sqlite3.h>

G_BEGIN_DECLS
    
void sys_db_init(const gchar *path);
void sys_db_dispose(void);
void sys_db_get_text(const gchar *name, gunichar2 *text);
    
G_END_DECLS

#endif /* __JM_SYSDB_H__ */