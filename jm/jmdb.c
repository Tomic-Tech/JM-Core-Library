#include "jmdb.h"
#include "jmauth.h"
#include <sqlite/sqlite3.h>

static const gchar* _jm_db_text_query = "SELECT TextContent FROM Text WHERE TextName=:textname AND Language=:language";
static const gchar* _jm_db_trouble_code_query = "SELECT Content FROM TroubleCode WHERE Code=:codename AND Catalog=:catalog AND Language=:language";
static const gchar* _jm_db_command_query = "SELECT Command FROM Command WHERE CommandName=:commandname AND Catalog=:catalog";
static const gchar* _jm_db_command_by_id_query = "SELECT Command FROM Command WHERE CommandID=:id";
static const gchar* _jm_db_live_data_query = "SELECT ShortName, Content, Unit, DefaultValue, CommandID, AlgorithmID FROM LiveData WHERE Catalog=:catalog AND Language=:language";
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

    if (sqlite3_open_v2(file_path, &_jm_db_handle, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
    {
        _jm_db_handle = NULL;
        return FALSE;
    }

    sqlite3_busy_timeout(_jm_db_handle, 5000);
    sqlite3_key(_jm_db_handle, password, g_utf8_strlen(password, -1));
    if ((sqlite3_prepare_v2(_jm_db_handle, _jm_db_text_query, g_utf8_strlen(_jm_db_text_query, -1), &_jm_db_get_text_stmt, NULL) != SQLITE_OK) ||
        (sqlite3_prepare_v2(_jm_db_handle, _jm_db_trouble_code_query, g_utf8_strlen(_jm_db_trouble_code_query, -1), &_jm_db_get_trouble_code_stmt, NULL) != SQLITE_OK) ||
        (sqlite3_prepare_v2(_jm_db_handle, _jm_db_command_query, g_utf8_strlen(_jm_db_command_query, -1), &_jm_db_get_command_stmt, NULL) != SQLITE_OK) ||
        (sqlite3_prepare_v2(_jm_db_handle, _jm_db_command_by_id_query, g_utf8_strlen(_jm_db_command_by_id_query, -1), &_jm_db_get_command_by_id_stmt, NULL) != SQLITE_OK) ||
        (sqlite3_prepare_v2(_jm_db_handle, _jm_db_live_data_query, g_utf8_strlen(_jm_db_live_data_query, -1), &_jm_db_get_live_data_stmt, NULL) != SQLITE_OK))
    {
        sqlite3_finalize(_jm_db_get_text_stmt);
        sqlite3_finalize(_jm_db_get_trouble_code_stmt);
        sqlite3_finalize(_jm_db_get_command_stmt);
        sqlite3_finalize(_jm_db_get_command_by_id_stmt);
        sqlite3_finalize(_jm_db_get_live_data_stmt);
        _jm_db_get_text_stmt = NULL;
        _jm_db_get_trouble_code_stmt = NULL;
        _jm_db_get_command_stmt = NULL;
        _jm_db_get_command_by_id_stmt = NULL;
        _jm_db_get_live_data_stmt = NULL;
        sqlite3_close(_jm_db_handle);
        _jm_db_handle = NULL;
        return FALSE;
    }
    return TRUE;
}

gboolean jm_db_close(void)
{
    int ret;
    sqlite3_stmt *stmt = NULL;
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
    gchar* lang = jm_auth_de_lang();
    GString *result = g_string_new(name);
    if ((sqlite3_reset(_jm_db_get_text_stmt) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_text_stmt, 1, name, g_utf8_strlen(name, -1), SQLITE_STATIC) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_text_stmt, 2, lang, g_utf8_strlen(lang, -1), SQLITE_STATIC) == SQLITE_OK) && 
        (sqlite3_step(_jm_db_get_text_stmt) == SQLITE_ROW))
    {
        const char* text = (const char*)sqlite3_column_text(_jm_db_get_text_stmt, 0);
        result = g_string_assign(result, text ? text : "");
    }
    g_free(lang);
    return g_string_free(result, FALSE);
}

gchar *jm_db_get_trouble_code(const gchar *code)
{
    gchar* lang = jm_auth_de_lang();
    GString *result = g_string_new(code);

    if ((sqlite3_reset(_jm_db_get_trouble_code_stmt) == SQLITE_OK) &&
        (sqlite3_bind_text(_jm_db_get_trouble_code_stmt, 1, code, g_utf8_strlen(code, -1), SQLITE_STATIC) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_trouble_code_stmt, 2, _jm_db_tc_catalog->str, _jm_db_tc_catalog->len, SQLITE_STATIC) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_trouble_code_stmt, 3, lang, g_utf8_strlen(lang, -1), SQLITE_STATIC) == SQLITE_OK))
    {
        int rc = sqlite3_step(_jm_db_get_trouble_code_stmt);
        if (rc == SQLITE_ROW)
        {
            const char* text = (const char*)sqlite3_column_text(_jm_db_get_trouble_code_stmt, 0);
            result = g_string_assign(result, text ? text : "Undefined");
        }
        else if (rc == SQLITE_DONE)
        {
            result = g_string_assign(result, "Undefined");
        }
    }
    g_free(lang);
    return g_string_free(result, FALSE);
}

GByteArray *jm_db_get_command(const gchar *name)
{
    GByteArray *result = NULL;
    if ((sqlite3_reset(_jm_db_get_command_stmt) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_command_stmt, 1, name, g_utf8_strlen(name, -1), SQLITE_STATIC) == SQLITE_OK) &&
        (sqlite3_bind_text(_jm_db_get_command_stmt, 2, _jm_db_cmd_catalog->str, _jm_db_cmd_catalog->len, SQLITE_STATIC) == SQLITE_OK) && 
        (sqlite3_step(_jm_db_get_command_stmt) == SQLITE_ROW))
    {
        const guint8* bytes = (const guint8*) sqlite3_column_blob(_jm_db_get_command_stmt, 0);
        gint32 size = sqlite3_column_bytes(_jm_db_get_command_stmt, 0);
        result = g_byte_array_sized_new(size);
        result = g_byte_array_append(result, bytes, size);
    }
    return result;
}

GByteArray *jm_db_get_command_id(gint32 id)
{
    GByteArray *result = NULL;
    if ((sqlite3_reset(_jm_db_get_command_by_id_stmt) == SQLITE_OK) &&
        (sqlite3_bind_int(_jm_db_get_command_by_id_stmt, 1, id) == SQLITE_OK) &&
        (sqlite3_step(_jm_db_get_command_by_id_stmt) == SQLITE_ROW))
    {
        const guint8 *bytes = (const guint8*) sqlite3_column_blob(_jm_db_get_command_by_id_stmt, 0);
        gint32 size = sqlite3_column_bytes(_jm_db_get_command_by_id_stmt, 0);
        result = g_byte_array_sized_new(size);
        result = g_byte_array_append(result, bytes, size);
    }
    return result;
}
JMLDArray *jm_db_get_live_data(void)
{
    gchar *lang = jm_auth_de_lang();
    JMLDArray *ret = jm_ld_array_new();

    if ((sqlite3_reset(_jm_db_get_live_data_stmt) == SQLITE_OK) &&
        (sqlite3_bind_text(_jm_db_get_live_data_stmt, 1, _jm_db_ld_catalog->str, _jm_db_ld_catalog->len, SQLITE_STATIC) == SQLITE_OK) && 
        (sqlite3_bind_text(_jm_db_get_live_data_stmt, 2, lang, g_utf8_strlen(lang, -1), SQLITE_STATIC) == SQLITE_OK))
    {
        while (sqlite3_step(_jm_db_get_live_data_stmt) == SQLITE_ROW)
        {
            const char *short_name = (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 0);
            const char *content = (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 1);
            const char *unit = (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 2);
            const char *default_value = (const char*)sqlite3_column_text(_jm_db_get_live_data_stmt, 3);
            gint32 id = sqlite3_column_int(_jm_db_get_live_data_stmt, 4);
            gboolean enabled = TRUE;
            JMLiveData *ld = jm_live_data_new(short_name ? short_name : "",
                content ? content : "",
                unit ? unit : "",
                default_value ? default_value : "",
                id);
            jm_ld_array_append(ret, ld);
        }
    }
    return ret;
}
