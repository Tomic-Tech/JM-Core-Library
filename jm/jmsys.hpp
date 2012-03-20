#ifndef __JM_SYS_HPP__
#define __JM_SYS_HPP__

#include <sqlite/sqlite3.h>
#include <glib.h>
#include "jmlib.h"

namespace JM
{
    class System
    {
    public:
        static System& inst()
        {
            static System instance;
            return instance;
        }

        void setPath(const gchar *path)
        {
            _path = g_string_new(path);
        }

        gchar* text(const gchar *name)
        {
            GString *result = g_string_new(name);
            if (_db == NULL)
            {
                int ret;
                GString *dbPath = g_string_new(_path->str);
                dbPath = g_string_append_c(dbPath, G_DIR_SEPARATOR);
                dbPath = g_string_append(dbPath, "sys.db");

                ret = sqlite3_open_v2(dbPath->str, &_db, SQLITE_OPEN_READONLY, NULL);
                g_string_free(dbPath, TRUE);
                if (ret != SQLITE_OK)
                {
                    _db = NULL;
                    goto END;
                }
            }

            if (_getTextStmt == NULL)
            {
                gchar *lang = jm_auth_decrypt_lang();
                int ret = 0;
                GString *temp = g_string_new("SELECT Content FROM [Text");
                temp = g_string_append(temp, lang);
                g_free(lang);
                temp = g_string_append(temp, "] WHERE Name=:name");
                ret = sqlite3_prepare_v2(_db, temp->str, temp->len, &_getTextStmt, NULL);
                g_string_free(temp, TRUE);
                if (ret != SQLITE_OK)
                {
                    sqlite3_finalize(_getTextStmt);
                    _getTextStmt = NULL;
                    goto END;
                }
            }

            if ((sqlite3_reset(_getTextStmt) == SQLITE_OK) && 
                (sqlite3_bind_text(_getTextStmt, 1, name, g_utf8_strlen(name, -1), SQLITE_STATIC) == SQLITE_OK) &&  
                (sqlite3_step(_getTextStmt) == SQLITE_ROW))
            {
                const char* temp = (const char*)sqlite3_column_text(_getTextStmt, 0);
                result = g_string_assign(result, temp ? temp : "");
            }

END:
            return g_string_free(result, FALSE);
        }
    private:
        System()
            : _path(NULL)
            , _db(NULL)
            , _getTextStmt(NULL)
        {
            sqlite3_enable_shared_cache(TRUE);
        }

        ~System()
        {
            sqlite3_stmt *stmt = NULL;
            while ((stmt = sqlite3_next_stmt(_db, 0)) != NULL)
            {
                sqlite3_finalize(stmt);
            }
            sqlite3_close(_db);
            _db = NULL;
        }
    private:
        GString *_path;
        sqlite3 *_db;
        sqlite3_stmt *_getTextStmt;
    };
}

#endif

