#include "jmsys.h"
#include "sqlite/sqlite3.h"
#include "jmauth.h"

static GString *_jm_sys_software_path = NULL;
static sqlite3* _jm_sys_db = NULL;
static sqlite3_stmt* _jm_sys_get_text_stmt = NULL;

void jm_sys_init(void)
{
    sqlite3_enable_shared_cache(TRUE);
}

void jm_sys_destroy(void)
{
    if (_jm_sys_db != NULL)
    {
        sqlite3_stmt *stmt = NULL;
        while ((stmt = sqlite3_next_stmt(_jm_sys_db, 0)) != NULL)
        {
            sqlite3_finalize(stmt);
        }
        sqlite3_close(_jm_sys_db);
        _jm_sys_db = NULL;
    }
}

void jm_sys_set_software_path(const gchar *software_path)
{
    _jm_sys_software_path = g_string_new(software_path);
}

gchar* jm_sys_text(const gchar *name)
{
    GString *result = g_string_new(name);
    if (_jm_sys_db == NULL)
    {
        int ret;
        GString *db_path = g_string_new(_jm_sys_software_path->str);
        db_path = g_string_append_c(db_path, G_DIR_SEPARATOR);
        db_path = g_string_append(db_path, "sys.db");

        ret = sqlite3_open_v2(db_path->str, &_jm_sys_db, 
            SQLITE_OPEN_READONLY, NULL);
        g_string_free(db_path, TRUE);
        if (ret != SQLITE_OK)
        {
            _jm_sys_db = NULL;
            goto END;
        }

    }

    if (_jm_sys_get_text_stmt == NULL)
    {
        gchar *lang = jm_auth_decrypt_lang();
        int ret = 0;
        GString *text = g_string_new("SELECT Content FROM [Text");
        text = g_string_append(text, lang);
        g_free(lang);
        text = g_string_append(text, "] WHERE Name=:name");
        ret = sqlite3_prepare_v2(_jm_sys_db, text->str, text->len, 
            &_jm_sys_get_text_stmt, NULL);
        g_string_free(text, TRUE);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_jm_sys_get_text_stmt);
            _jm_sys_get_text_stmt = NULL;
            goto END;
        }
    }

    if ((sqlite3_reset(_jm_sys_get_text_stmt) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_sys_get_text_stmt, 1, name, 
        g_utf8_strlen(name, -1), SQLITE_STATIC) == SQLITE_OK) &&  
        (sqlite3_step(_jm_sys_get_text_stmt) == SQLITE_ROW))
    {
        const char* text = 
            (const char*)sqlite3_column_text(_jm_sys_get_text_stmt, 0);
        result = g_string_assign(result, text ? text : "");
    }

END:
    return g_string_free(result, FALSE);
}
