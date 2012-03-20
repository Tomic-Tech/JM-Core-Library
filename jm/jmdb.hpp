#ifndef __JM_DB_HPP__
#define __JM_DB_HPP__

#include <sqlite/sqlite3.h>
#include <glib.h>
#include "jmldarray.h"
#include "jmlib.h"

namespace JM
{
    class Database
    {
    public:
        static Database& inst()
        {
            static Database instance;
            return instance;
        }

        gboolean open(const gchar *filePath, const gchar *password)
        {
            close();

            if (sqlite3_open_v2(filePath, &_handle, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
            {
                _handle = NULL;
                return FALSE;
            }

            sqlite3_busy_timeout(_handle, 5000);
            sqlite3_key(_handle, password, g_utf8_strlen(password, -1));
            return TRUE;
        }

        gboolean close()
        {
            int ret;
            sqlite3_stmt *stmt = NULL;
            _getTextStmt = NULL;
            _getTroubleCodeStmt = NULL;
            _getCommandStmt = NULL;
            _getCommandByIDStmt = NULL;
            _getLiveDataStmt = NULL;

            if (_handle == NULL)
            {
                return TRUE;
            }
            while ((stmt = sqlite3_next_stmt(_handle, 0)) != NULL)
            {
                ret = sqlite3_finalize(stmt);
                if (ret != SQLITE_OK)
                {
                    return FALSE;
                }
            }
            ret = sqlite3_close(_handle);
            if (ret != SQLITE_OK)
            {
                return FALSE;
            }
            _handle = NULL;
            return TRUE;
        }

        gboolean isOpen()
        {
            return _handle != NULL ? TRUE : FALSE;
        }

        void setTCCatalog(const gchar *catalog)
        {
            _tcCatalog = g_string_assign(_tcCatalog, catalog);
        }

        void setLDCatalog(const gchar *catalog)
        {
            _ldCatalog = g_string_assign(_ldCatalog, catalog);
        }

        void setCMDCatalog(const gchar *catalog)
        {
            _cmdCatalog = g_string_assign(_cmdCatalog, catalog);
        }

        gchar* getText(const gchar *name)
        {
            GString *result = g_string_new(name);
            if (_getTextStmt == NULL)
            {
                gchar* lang = jm_auth_decrypt_lang();
                int ret = 0;
                GString *text = g_string_new("SELECT Content FROM [Text");
                text = g_string_append(text, lang);
                g_free(lang);
                text = g_string_append(text, "] WHERE Name=:name");
                ret = sqlite3_prepare_v2(_handle, text->str, text->len, &_getTextStmt, NULL);
                g_string_free(text, TRUE);
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
                const char* text = 
                    (const char*)sqlite3_column_text(_getTextStmt, 0);
                result = g_string_assign(result, text ? text : "");
            }

END:
            return g_string_free(result, FALSE);
        }

        gchar* getTroubleCode(const gchar *code)
        {
            GString *result = g_string_new(code);

            if (_getTroubleCodeStmt == NULL)
            {
                gchar* lang = jm_auth_decrypt_lang();
                int ret = 0;
                GString *text = g_string_new("SELECT Content FROM [TroubleCode");
                text = g_string_append(text, lang);
                text = g_string_append(text, "] WHERE Code=:code AND Catalog=:catalog");
                ret = sqlite3_prepare_v2(_handle, text->str, text->len, &_getTroubleCodeStmt, NULL);
                g_free(lang);
                g_string_free(text, TRUE);
                if (ret != SQLITE_OK)
                {
                    sqlite3_finalize(_getTroubleCodeStmt);
                    _getTroubleCodeStmt = NULL;
                    goto END;
                }
            }

            if ((sqlite3_reset(_getTroubleCodeStmt) == SQLITE_OK) &&
                (sqlite3_bind_text(_getTroubleCodeStmt, 1, code, g_utf8_strlen(code, -1), SQLITE_STATIC) == SQLITE_OK) && 
                (sqlite3_bind_text(_getTroubleCodeStmt, 2, _tcCatalog->str, _tcCatalog->len, SQLITE_STATIC) == SQLITE_OK))
            {
                int rc = sqlite3_step(_getTroubleCodeStmt);
                if (rc == SQLITE_ROW)
                {
                    const char* text = (const char*)sqlite3_column_text(_getTroubleCodeStmt, 0);
                    result = g_string_assign(result, text ? text : "Undefined");
                }
                else if (rc == SQLITE_DONE)
                {
                    result = g_string_assign(result, "Undefined");
                }
            }

END:
            return g_string_free(result, FALSE);
        }

        GByteArray* getCommand(const gchar *name)
        {
            GByteArray *result = NULL;
            if (_getCommandStmt == NULL)
            {
                static gchar *text = "SELECT Command FROM [Command] WHERE Name=:name AND Catalog=:catalog";
                int ret;
                ret = sqlite3_prepare_v2(_handle, text, g_utf8_strlen(text, -1), &_getCommandStmt, NULL);
                if (ret != SQLITE_OK)
                {
                    sqlite3_finalize(_getCommandStmt);
                    _getCommandStmt = NULL;
                    goto END;
                }
            }
            if ((sqlite3_reset(_getCommandStmt) == SQLITE_OK) && 
                (sqlite3_bind_text(_getCommandStmt, 1, name, g_utf8_strlen(name, -1), SQLITE_STATIC) == SQLITE_OK) &&
                (sqlite3_bind_text(_getCommandStmt, 2, _cmdCatalog->str, _cmdCatalog->len, SQLITE_STATIC) == SQLITE_OK) && 
                (sqlite3_step(_getCommandStmt) == SQLITE_ROW))
            {
                const guint8* bytes = (const guint8*) sqlite3_column_blob(_getCommandStmt, 0);
                gint32 size = sqlite3_column_bytes(_getCommandStmt, 0);
                result = g_byte_array_sized_new(size);
                result = g_byte_array_append(result, bytes, size);
            }
END:
            return result;
        }

        GByteArray* getCommandByID(gint32 id)
        {
            GByteArray *result = NULL;
            if (_getCommandByIDStmt == NULL)
            {
                static gchar *text = "SELECT Command FROM [Command] WHERE ID=:id";
                int ret = sqlite3_prepare_v2(_handle, text, g_utf8_strlen(text, -1), &_getCommandByIDStmt, NULL);
                if (ret != SQLITE_OK)
                {
                    sqlite3_finalize(_getCommandByIDStmt);
                    _getCommandByIDStmt = NULL;
                    goto END;
                }
            }

            if ((sqlite3_reset(_getCommandByIDStmt) == SQLITE_OK) &&
                (sqlite3_bind_int(_getCommandByIDStmt, 1, id) == SQLITE_OK) && 
                (sqlite3_step(_getCommandByIDStmt) == SQLITE_ROW))
            {
                const guint8 *bytes = (const guint8*) sqlite3_column_blob(_getCommandByIDStmt, 0);
                gint32 size = sqlite3_column_bytes(_getCommandByIDStmt, 0);
                result = g_byte_array_sized_new(size);
                result = g_byte_array_append(result, bytes, size);
            }

END:
            return result;
        }

        JMLDArray* getLiveData(gboolean showed)
        {
            JMLDArray *ret = jm_ld_array_new();

            if (_getLiveDataStmt == NULL)
            {
                int ret;
                gchar *lang = jm_auth_decrypt_lang();
                GString *text = g_string_new("SELECT ShortName, Content, Unit, DefaultValue, CommandID, AlgorithmID FROM [LiveData");
                text = g_string_append(text, lang);
                text = g_string_append(text, "] WHERE Catalog=:catalog");
                ret = sqlite3_prepare_v2(_handle, text->str, text->len, &_getLiveDataStmt, NULL);
                g_free(lang);
                g_string_free(text, TRUE);
                if (ret != SQLITE_OK)
                {
                    sqlite3_finalize(_getLiveDataStmt);
                    _getLiveDataStmt = NULL;
                    goto END;
                }
            }

            if ((sqlite3_reset(_getLiveDataStmt) == SQLITE_OK) &&
                (sqlite3_bind_text(_getLiveDataStmt, 1, _ldCatalog->str, _ldCatalog->len, SQLITE_STATIC) == SQLITE_OK))
            {
                while (sqlite3_step(_getLiveDataStmt) == SQLITE_ROW)
                {
                    const char *short_name = (const char*)sqlite3_column_text(_getLiveDataStmt, 0);
                    const char *content = (const char*)sqlite3_column_text(_getLiveDataStmt, 1);
                    const char *unit = (const char*)sqlite3_column_text(_getLiveDataStmt, 2);
                    const char *default_value = (const char*)sqlite3_column_text(_getLiveDataStmt, 3);
                    gint32 id = sqlite3_column_int(_getLiveDataStmt, 4);
                    gboolean enabled = TRUE;
                    JMLiveData *ld = jm_live_data_new(short_name ? short_name : "",
                        content ? content : "",
                        unit ? unit : "",
                        default_value ? default_value : "",
                        id,
                        showed);
                    jm_ld_array_append(ret, ld);
                }
            }

END:
            return ret;
        }

    private:
        Database()
            : _tcCatalog(g_string_new(""))
            , _ldCatalog(g_string_new(""))
            , _cmdCatalog(g_string_new(""))
            , _handle(NULL)
            , _getTextStmt(NULL)
            , _getTroubleCodeStmt(NULL)
            , _getCommandStmt(NULL)
            , _getCommandByIDStmt(NULL)
            , _getLiveDataStmt(NULL)
        {
            sqlite3_enable_shared_cache(TRUE);
        }

        ~Database()
        {
            close();
            g_string_free(_tcCatalog, TRUE);
            g_string_free(_ldCatalog, TRUE);
            g_string_free(_cmdCatalog, TRUE);
        }
    private:
        GString *_tcCatalog;
        GString *_ldCatalog;
        GString *_cmdCatalog;
        sqlite3 *_handle;
        sqlite3_stmt *_getTextStmt;
        sqlite3_stmt *_getTroubleCodeStmt;
        sqlite3_stmt *_getCommandStmt;
        sqlite3_stmt *_getCommandByIDStmt;
        sqlite3_stmt *_getLiveDataStmt;
    };
}

#endif