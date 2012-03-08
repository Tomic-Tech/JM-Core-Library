#include "jmdb.h"
#include "jmauth.h"
#include <sqlite/sqlite3.h>

static GString* _jm_db_tc_catalog = NULL;
static GString* _jm_db_ld_catalog = NULL;
static GString* _jm_db_cmd_catalog = NULL;
static sqlite3* _jm_db_handle = NULL;
static sqlite3_stmt* _jm_db_get_text_stmt = NULL;
static sqlite3_stmt* _jm_db_get_trouble_code_stmt = NULL;
static sqlite3_stmt* _jm_db_get_command_stmt = NULL;
static sqlite3_stmt* _jm_db_get_command_by_id_stmt = NULL;
static sqlite3_stmt* _jm_db_get_live_data_stmt = NULL;

void jm_db_init(void)
{
    sqlite3_enable_shared_cache(TRUE);
    _jm_db_tc_catalog = g_string_new("");
    _jm_db_ld_catalog = g_string_new("");
    _jm_db_cmd_catalog = g_string_new("");
}

void jm_db_destroy(void)
{
    jm_db_close();
    g_string_free(_jm_db_tc_catalog, TRUE);
    g_string_free(_jm_db_ld_catalog, TRUE);
    g_string_free(_jm_db_cmd_catalog, TRUE);
}

gboolean jm_db_open(const gchar* file_path, const gchar* password)
{
    jm_db_close();

    if (sqlite3_open_v2(file_path, &_jm_db_handle, 
        SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
    {
        _jm_db_handle = NULL;
        return FALSE;
    }

    sqlite3_busy_timeout(_jm_db_handle, 5000);
    sqlite3_key(_jm_db_handle, password, g_utf8_strlen(password, -1));
    //if ((sqlite3_prepare_v2(_jm_db_handle, _jm_db_text_query, g_utf8_strlen(_jm_db_text_query, -1), &_jm_db_get_text_stmt, NULL) != SQLITE_OK) ||
    //    (sqlite3_prepare_v2(_jm_db_handle, _jm_db_trouble_code_query, g_utf8_strlen(_jm_db_trouble_code_query, -1), &_jm_db_get_trouble_code_stmt, NULL) != SQLITE_OK) ||
    //    (sqlite3_prepare_v2(_jm_db_handle, _jm_db_command_query, g_utf8_strlen(_jm_db_command_query, -1), &_jm_db_get_command_stmt, NULL) != SQLITE_OK) ||
    //    (sqlite3_prepare_v2(_jm_db_handle, _jm_db_command_by_id_query, g_utf8_strlen(_jm_db_command_by_id_query, -1), &_jm_db_get_command_by_id_stmt, NULL) != SQLITE_OK) ||
    //    (sqlite3_prepare_v2(_jm_db_handle, _jm_db_live_data_query, g_utf8_strlen(_jm_db_live_data_query, -1), &_jm_db_get_live_data_stmt, NULL) != SQLITE_OK))
    //{
    //    sqlite3_finalize(_jm_db_get_text_stmt);
    //    sqlite3_finalize(_jm_db_get_trouble_code_stmt);
    //    sqlite3_finalize(_jm_db_get_command_stmt);
    //    sqlite3_finalize(_jm_db_get_command_by_id_stmt);
    //    sqlite3_finalize(_jm_db_get_live_data_stmt);
    //    _jm_db_get_text_stmt = NULL;
    //    _jm_db_get_trouble_code_stmt = NULL;
    //    _jm_db_get_command_stmt = NULL;
    //    _jm_db_get_command_by_id_stmt = NULL;
    //    _jm_db_get_live_data_stmt = NULL;
    //    sqlite3_close(_jm_db_handle);
    //    _jm_db_handle = NULL;
    //    return FALSE;
    //}
    return TRUE;
}

gboolean jm_db_close(void)
{
    int ret;
    sqlite3_stmt *stmt = NULL;
    _jm_db_get_text_stmt = NULL;
    _jm_db_get_trouble_code_stmt = NULL;
    _jm_db_get_command_stmt = NULL;
    _jm_db_get_command_by_id_stmt = NULL;
    _jm_db_get_live_data_stmt = NULL;

    if (_jm_db_handle == NULL)
    {
        return TRUE;
    }
    while ((stmt = sqlite3_next_stmt(_jm_db_handle, 0)) != NULL)
    {
        ret = sqlite3_finalize(stmt);
        if (ret != SQLITE_OK)
        {
            return FALSE;
        }
    }
    ret = sqlite3_close(_jm_db_handle);
    if (ret != SQLITE_OK)
    {
        return FALSE;
    }
    _jm_db_handle = NULL;
    return TRUE;
}

gboolean jm_db_is_open(void)
{
    return _jm_db_handle != NULL ? TRUE : FALSE;
}

void jm_db_set_tc_catalog(const gchar *catalog)
{
    _jm_db_tc_catalog = g_string_assign(_jm_db_tc_catalog, catalog);
}

void jm_db_set_ld_catalog(const gchar *catalog)
{
    _jm_db_ld_catalog = g_string_assign(_jm_db_ld_catalog, catalog);
}

void jm_db_set_cmd_catalog(const gchar *catalog)
{
    _jm_db_cmd_catalog = g_string_assign(_jm_db_cmd_catalog, catalog);
}

gchar *jm_db_get_text(const gchar *name)
{
    GString *result = g_string_new(name);
    if (_jm_db_get_text_stmt == NULL)
    {
        gchar* lang = jm_auth_decrypt_lang();
        int ret = 0;
        GString *text = g_string_new("SELECT Content FROM [Text");
        text = g_string_append(text, lang);
        g_free(lang);
        text = g_string_append(text, "] WHERE Name=:name");
        ret = sqlite3_prepare_v2(_jm_db_handle, text->str, text->len, 
            &_jm_db_get_text_stmt, NULL);
        g_string_free(text, TRUE);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_jm_db_get_text_stmt);
            _jm_db_get_text_stmt = NULL;
            goto END;
        }
    }

    if ((sqlite3_reset(_jm_db_get_text_stmt) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_text_stmt, 1, name, 
        g_utf8_strlen(name, -1), SQLITE_STATIC) == SQLITE_OK) &&  
        (sqlite3_step(_jm_db_get_text_stmt) == SQLITE_ROW))
    {
        const char* text = 
            (const char*)sqlite3_column_text(_jm_db_get_text_stmt, 0);
        result = g_string_assign(result, text ? text : "");
    }

END:
    return g_string_free(result, FALSE);
}

gchar *jm_db_get_trouble_code(const gchar *code)
{
    GString *result = g_string_new(code);

    if (_jm_db_get_trouble_code_stmt == NULL)
    {
        gchar* lang = jm_auth_decrypt_lang();
        int ret = 0;
        GString *text = g_string_new("SELECT Content FROM [TroubleCode");
        text = g_string_append(text, lang);
        text = g_string_append(text, 
            "] WHERE Code=:code AND Catalog=:catalog");
        ret = sqlite3_prepare_v2(_jm_db_handle, text->str, text->len, 
            &_jm_db_get_trouble_code_stmt, NULL);
        g_free(lang);
        g_string_free(text, TRUE);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_jm_db_get_trouble_code_stmt);
            _jm_db_get_trouble_code_stmt = NULL;
            goto END;
        }
    }

    if ((sqlite3_reset(_jm_db_get_trouble_code_stmt) == SQLITE_OK) &&
        (sqlite3_bind_text(_jm_db_get_trouble_code_stmt, 1, code, 
        g_utf8_strlen(code, -1), SQLITE_STATIC) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_trouble_code_stmt, 2, 
        _jm_db_tc_catalog->str, _jm_db_tc_catalog->len, SQLITE_STATIC) == 
        SQLITE_OK))
    {
        int rc = sqlite3_step(_jm_db_get_trouble_code_stmt);
        if (rc == SQLITE_ROW)
        {
            const char* text = 
                (const char*)sqlite3_column_text(
                _jm_db_get_trouble_code_stmt, 0);
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

GByteArray *jm_db_get_command(const gchar *name)
{
    GByteArray *result = NULL;
    if (_jm_db_get_command_stmt == NULL)
    {
        static gchar *text = 
            "SELECT Command FROM [Command] WHERE Name=:name AND "
            "Catalog=:catalog";
        int ret;
        ret = sqlite3_prepare_v2(_jm_db_handle, text, 
            g_utf8_strlen(text, -1), &_jm_db_get_command_stmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_jm_db_get_command_stmt);
            _jm_db_get_command_stmt = NULL;
            goto END;
        }
    }
    if ((sqlite3_reset(_jm_db_get_command_stmt) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_command_stmt, 1, name, 
        g_utf8_strlen(name, -1), SQLITE_STATIC) == SQLITE_OK) &&
        (sqlite3_bind_text(_jm_db_get_command_stmt, 2, 
        _jm_db_cmd_catalog->str, _jm_db_cmd_catalog->len, SQLITE_STATIC) == 
        SQLITE_OK) && (sqlite3_step(_jm_db_get_command_stmt) == SQLITE_ROW))
    {
        const guint8* bytes = 
            (const guint8*) sqlite3_column_blob(_jm_db_get_command_stmt, 0);
        gint32 size = sqlite3_column_bytes(_jm_db_get_command_stmt, 0);
        result = g_byte_array_sized_new(size);
        result = g_byte_array_append(result, bytes, size);
    }
END:
    return result;
}

GByteArray *jm_db_get_command_by_id(gint32 id)
{
    GByteArray *result = NULL;
    if (_jm_db_get_command_by_id_stmt == NULL)
    {
        static gchar *text = "SELECT Command FROM [Command] WHERE ID=:id";
        int ret = sqlite3_prepare_v2(_jm_db_handle, text, 
            g_utf8_strlen(text, -1), &_jm_db_get_command_by_id_stmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_jm_db_get_command_by_id_stmt);
            _jm_db_get_command_by_id_stmt = NULL;
            goto END;
        }
    }

    if ((sqlite3_reset(_jm_db_get_command_by_id_stmt) == SQLITE_OK) &&
        (sqlite3_bind_int(_jm_db_get_command_by_id_stmt, 1, id) 
        == SQLITE_OK) && (sqlite3_step(_jm_db_get_command_by_id_stmt) == 
        SQLITE_ROW))
    {
        const guint8 *bytes = 
            (const guint8*) sqlite3_column_blob(
            _jm_db_get_command_by_id_stmt, 0);
        gint32 size = sqlite3_column_bytes(_jm_db_get_command_by_id_stmt, 
            0);
        result = g_byte_array_sized_new(size);
        result = g_byte_array_append(result, bytes, size);
    }

END:
    return result;
}
JMLDArray *jm_db_get_live_data(gboolean showed)
{
    JMLDArray *ret = jm_ld_array_new();

    if (_jm_db_get_live_data_stmt == NULL)
    {
        int ret;
        gchar *lang = jm_auth_decrypt_lang();
        GString *text = g_string_new("SELECT ShortName, Content, Unit, "
            "DefaultValue, CommandID, AlgorithmID FROM [LiveData");
        text = g_string_append(text, lang);
        text = g_string_append(text, "] WHERE Catalog=:catalog");
        ret = sqlite3_prepare_v2(_jm_db_handle, text->str, text->len, 
            &_jm_db_get_live_data_stmt, NULL);
        g_free(lang);
        g_string_free(text, TRUE);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_jm_db_get_live_data_stmt);
            _jm_db_get_live_data_stmt = NULL;
            goto END;
        }
    }

    if ((sqlite3_reset(_jm_db_get_live_data_stmt) == SQLITE_OK) &&
        (sqlite3_bind_text(_jm_db_get_live_data_stmt, 1, 
        _jm_db_ld_catalog->str, _jm_db_ld_catalog->len, SQLITE_STATIC) == 
        SQLITE_OK))
    {
        while (sqlite3_step(_jm_db_get_live_data_stmt) == SQLITE_ROW)
        {
            const char *short_name = 
                (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 
                0);
            const char *content = 
                (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 
                1);
            const char *unit = 
                (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 
                2);
            const char *default_value = 
                (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 
                3);
            gint32 id = sqlite3_column_int(_jm_db_get_live_data_stmt, 4);
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
