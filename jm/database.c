#include "database.h"
#include "register.h"

static gchar dbPath[1024];
static sqlite3 *db = NULL;
static sqlite3_stmt *getTextStmt = NULL;
static sqlite3_stmt *getTroubleCodeStmt = NULL;
static sqlite3_stmt *getLiveDataStmt = NULL;
static sqlite3_stmt *getCommandByIdStmt = NULL;
static sqlite3_stmt *getCommandStmt = NULL;
static char language[128] = {0};
static GString *liveDataClass = NULL;

void database_init(const gchar *path)
{
	int ret;
	gchar password[128] = { 0 };

	sqlite3_enable_shared_cache(1);
	
	g_stpcpy(dbPath, path);
	g_stpcpy(dbPath + strlen(path), "sys.db");

	ret = sqlite3_open_v2(dbPath, &db, SQLITE_OPEN_READONLY, NULL);

	if (ret != SQLITE_OK)
		return;

	if (register_decrypt(DatabasePassword, password))
		sqlite3_key(db, password, strlen(password));

	register_decrypt(Language, language);
	liveDataClass = g_string_new("");
}

void database_dispose(void)
{
	sqlite3_stmt *stmt = NULL;
	while ((stmt = sqlite3_next_stmt(db, 0)) != NULL) {
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);
	db = NULL;
	g_string_free(liveDataClass, TRUE);
}

//Database * database_new(const gchar *filePath) {
//	gchar password[128] = { 0 };
//	Database *obj = (Database*) g_malloc(sizeof(Database));
//	obj->tcCatalog = NULL;
//	obj->ldCatalog = NULL;
//	obj->cmdCatalog = NULL;
//	obj->handle = NULL;
//	obj->getTextStmt = NULL;
//	obj->getTroubleCodeStmt = NULL;
//	obj->getLiveDataStmt = NULL;
//	obj->getCommandByIDStmt = NULL;
//	obj->getCommandStmt = NULL;
//
//	sqlite3_enable_shared_cache(1);
//
//	if (sqlite3_open_v2(filePath, &obj->handle, SQLITE_OPEN_READONLY,
//			NULL) != SQLITE_OK) {
//		obj->handle = NULL;
//		g_free(obj);
//		return NULL;
//	}
//
//	register_decrypt(DatabasePassword, password);
//	sqlite3_busy_timeout(obj->handle, 5000);
//	sqlite3_key(obj->handle, password, strlen(password));
//
//	obj->tcCatalog = g_string_new("");
//	obj->ldCatalog = g_string_new("");
//	obj->cmdCatalog = g_string_new("");
//
//	return obj;
//}
//
//void database_dispose(Database *obj) {
//	sqlite3_stmt *stmt = NULL;
//	g_return_if_fail(obj != NULL);
//
//	while ((stmt = sqlite3_next_stmt(obj->handle, 0)) != NULL) {
//		sqlite3_finalize(stmt);
//	}
//
//	sqlite3_close(obj->handle);
//	g_string_free(obj->tcCatalog, TRUE);
//	g_string_free(obj->ldCatalog, TRUE);
//	g_string_free(obj->cmdCatalog, TRUE);
//
//	g_free(obj);
//}

//void database_set_tc_catalog(Database *obj, const gchar *catalog) {
//	g_return_if_fail(obj != NULL);
//	g_string_truncate(obj->tcCatalog, 0);
//	g_string_append(obj->tcCatalog, catalog);
//}
//
//void database_set_ld_catalog(Database *obj, const gchar *catalog) {
//	g_return_if_fail(obj != NULL);
//	g_string_truncate(obj->ldCatalog, 0);
//	g_string_append(obj->ldCatalog, catalog);
//}
//
//void database_set_cmd_catalog(Database *obj, const gchar *catalog) {
//	g_return_if_fail(obj != NULL);
//	g_string_truncate(obj->cmdCatalog, 0);
//	g_string_append(obj->cmdCatalog, catalog);
//}

gboolean database_get_text(const gchar *name, const gchar *cls, gunichar2 *text) {
	long items_read = 0;
	long items_written = 0;
	gunichar2 *temp = NULL;

	temp = g_utf8_to_utf16(name, g_utf8_strlen(name, -1), &items_read,
			&items_written, NULL);
	memcpy(text, temp, sizeof(gunichar2) * (items_written + 1));
	g_free(temp);

	if (getTextStmt == NULL) {
		int ret = 0;
		gchar *temp = "SELECT Content FROM \"Text\" WHERE \"Name\"=:name AND \"Language\"=:language AND \"Class\"=:class";

		//temp = g_string_append(temp, lang);
		//temp = g_string_append(temp, "] WHERE Name=:name");

		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getTextStmt, NULL);
		if (ret != SQLITE_OK) {
			const char *msg = sqlite3_errmsg(db);
			sqlite3_finalize(getTextStmt);
			getTextStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(getTextStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(getTextStmt, 1, name, strlen(name), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTextStmt, 2, language, strlen(language), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTextStmt, 3, cls, strlen(cls), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_step(getTextStmt) == SQLITE_ROW)) {
		const gunichar2 *temp = (const gunichar2 *) sqlite3_column_text16(getTextStmt, 0);
		if (temp != NULL)
			memcpy(text, temp, sqlite3_column_bytes16(getTextStmt, 0)+ sizeof(gunichar2));
	}

	return TRUE;
}

gboolean database_get_trouble_code(const gchar *code, const gchar *cls, gunichar2 *content, gunichar2 *description) {
	long items_read = 0;
	long items_written = 0;
	gunichar2 *temp = NULL;
	temp = g_utf8_to_utf16(code, g_utf8_strlen(code, -1), &items_read, &items_written, NULL);
	memcpy(content, temp, sizeof(gunichar2) * (items_written + 1));
	g_free(temp);

	if (getTroubleCodeStmt == NULL) {
		int ret = 0;
		gchar *temp = "SELECT \"Content\", \"Description\" FROM \"TroubleCode\" WHERE \"Code\"=:code AND \"Language\"=:language AND \"Class\"=:class";

		//temp = g_string_append(temp, lang);
		//temp = g_string_append(temp, "] WHERE Code=:code AND Catalog=:catalog");

		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getTroubleCodeStmt, NULL);
		if (ret != SQLITE_OK) {
			sqlite3_finalize(getTroubleCodeStmt);
			getTroubleCodeStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(getTroubleCodeStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(getTroubleCodeStmt, 1, code, strlen(code), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTroubleCodeStmt, 2, language, strlen(language), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTroubleCodeStmt, 3, cls, strlen(cls), SQLITE_STATIC) == SQLITE_OK)) {
		int rc = sqlite3_step(getTroubleCodeStmt);
		if (rc == SQLITE_ROW) {
			const gunichar2* temp = (const gunichar2*) sqlite3_column_text16(getTroubleCodeStmt, 0);
			if (temp) {
				memcpy(content, temp, sqlite3_column_bytes16(getTroubleCodeStmt, 0) + sizeof(gunichar2));
			} else {
				database_get_text("Undefined", "System", content);
			}
			temp = (const gunichar2*) sqlite3_column_text16(getTroubleCodeStmt, 1);
			if (temp) {
				memcpy(description, temp, sqlite3_column_bytes16(getTroubleCodeStmt, 1) + sizeof(gunichar2));
			} else {
				database_get_text("Undefined", "System", description);
			}
		} else if (rc == SQLITE_DONE) {
			database_get_text("Undefined", "System", content);
			database_get_text("Undefined", "System", description);
		}
	}

	return TRUE;
}

gboolean database_live_data_prepare(const gchar *cls) {
	if (getLiveDataStmt == NULL) {
		//gchar *temp = "SELECT \"ShortName\", \"Content\", \"Unit\", \"DefaultValue\", \"CommandID\", \"Description\" FROM \"LiveData\" WHERE \"Language\"=:language AND \"Class\"=:class";
		//gchar *temp = "SELECT * FROM LiveData WHERE Language=:language AND Class=:class";
		gchar *temp = "SELECT \"ShortName\", \"Content\", \"Unit\", \"DefaultValue\", \"CommandID\", \"Description\" FROM LiveData WHERE \"Language\"=:language AND \"Class\"=:class";
		int ret = 0;

		//temp = g_string_append(temp, lang);
		//temp = g_string_append(temp, "] WHERE Catalog=:catalog");

		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getLiveDataStmt, NULL);

		if (ret != SQLITE_OK) {
			sqlite3_finalize(getLiveDataStmt);
			getLiveDataStmt = NULL;
			return FALSE;
		}
	}

	liveDataClass = g_string_set_size(liveDataClass, 0);
	liveDataClass = g_string_append(liveDataClass, cls);

	if ((sqlite3_reset(getLiveDataStmt) == SQLITE_OK)
		&& (sqlite3_bind_text(getLiveDataStmt, 1, language, strlen(language), SQLITE_STATIC) == SQLITE_OK)
		&& (sqlite3_bind_text(getLiveDataStmt, 2, liveDataClass->str, liveDataClass->len, SQLITE_STATIC) == SQLITE_OK)) {
		return TRUE;
	}

	return FALSE;
}

gboolean database_live_data_next(gunichar2 *shortName, gunichar2 *content, gunichar2 *unit, 
	gunichar2 *defaultValue, gint32 *cmdID, gunichar2* description)
{
	if (sqlite3_step(getLiveDataStmt) == SQLITE_ROW)
	{
		gunichar2 *temp = (gunichar2*)sqlite3_column_text16(getLiveDataStmt, 0);
		if (temp != NULL)
		{
			memcpy(shortName, temp, sqlite3_column_bytes16(getLiveDataStmt, 0) + sizeof(gunichar2));
		}
		else
		{
			shortName[0] = 0;
		}

		temp = (gunichar2*)sqlite3_column_text16(getLiveDataStmt, 1);
		if (temp != NULL)
		{
			memcpy(content, temp, sqlite3_column_bytes16(getLiveDataStmt, 1) + sizeof(gunichar2));
		}
		else
		{
			content[0] = 0;
		}
		
		temp = (gunichar2*)sqlite3_column_text16(getLiveDataStmt, 2);
		if (temp != NULL)
		{
			memcpy (unit, temp, sqlite3_column_bytes16(getLiveDataStmt, 2) + sizeof(gunichar2));
		}
		else
		{
			unit[0] = 0;
		}
		
		temp = (gunichar2*)sqlite3_column_text16(getLiveDataStmt, 3);
		if (temp != NULL)
		{
			memcpy (defaultValue, temp, sqlite3_column_bytes16(getLiveDataStmt, 3) + sizeof(gunichar2));
		}
		else
		{
			defaultValue[0] = 0;
		}
		
		*cmdID = sqlite3_column_int(getLiveDataStmt, 4);

		temp = (gunichar2*)sqlite3_column_text16(getLiveDataStmt, 5);
		if (temp != NULL)
		{
			memcpy (description, temp, sqlite3_column_bytes16(getLiveDataStmt, 5) + sizeof(gunichar2));
		}
		else
		{
			description[0] = 0;
		}
		return TRUE;
		//            const char *shortName = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 0);
		//            const char *content = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 1);
		//            const char *unit = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 2);
		//            const char *defaultValue = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 3);
		//            gint32 id = sqlite3_column_int(obj->getLiveDataStmt, 4);
		//            LiveData *ld = live_data_new(shortName ? shortName : "",
		//                content ? content : "",
		//                unit ? unit : "",
		//                defaultValue ? defaultValue : "",
		//                "",
		//                id);
		//            live_data_vector_push_back(vec, ld);
	}
	return FALSE;
}

//gboolean database_get_live_data(Database *obj, LiveDataVector *vec)
//{
//    g_return_val_if_fail(obj != NULL, FALSE);
//
//    if (obj->getLiveDataStmt == NULL) {
//        gchar lang[128];
//        GString *temp = g_string_new("SELECT ShortName, Content, Unit, DefaultValue, CommandID, AlgorithmID FROM [LiveData");
//        int ret = 0;
//
//        register_decrypt(Language, lang);
//        temp = g_string_append(temp, lang);
//        temp = g_string_append(temp, "] WHERE Catalog=:catalog");
//
//        ret = sqlite3_prepare_v2(obj->handle, temp->str, temp->len, &obj->getLiveDataStmt, NULL);
//        g_string_free(temp, TRUE);
//
//        if (ret != SQLITE_OK) {
//            sqlite3_finalize(obj->getLiveDataStmt);
//            obj->getLiveDataStmt = NULL;
//            return FALSE;
//        }
//    }
//
//    if ((sqlite3_reset(obj->getLiveDataStmt) == SQLITE_OK) &&
//        (sqlite3_bind_text(obj->getLiveDataStmt, 1, obj->ldCatalog->str, obj->ldCatalog->len, SQLITE_STATIC) == SQLITE_OK)) {
//
//        while (sqlite3_step(obj->getLiveDataStmt) == SQLITE_ROW) {
//            const char *shortName = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 0);
//            const char *content = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 1);
//            const char *unit = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 2);
//            const char *defaultValue = (const char*) sqlite3_column_text(obj->getLiveDataStmt, 3);
//            gint32 id = sqlite3_column_int(obj->getLiveDataStmt, 4);
//            LiveData *ld = live_data_new(shortName ? shortName : "",
//                content ? content : "",
//                unit ? unit : "",
//                defaultValue ? defaultValue : "",
//                "",
//                id);
//            live_data_vector_push_back(vec, ld);
//        }
//        return TRUE;
//    }
//
//    return FALSE;
//}

gboolean database_get_command(const gchar *name, const gchar *cls, guint8 *buffer, size_t *count) {
	if (getCommandStmt == NULL) {
		const char *temp = "SELECT \"Command\" FROM \"Command\" WHERE \"Name\"=:name AND \"Class\"=:catalog";
		int ret;
		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getCommandStmt, NULL);
		if (ret != SQLITE_OK) {
			sqlite3_finalize(getCommandStmt);
			getCommandStmt = NULL;
			return FALSE;
		}
	}
	if ((sqlite3_reset(getCommandStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(getCommandStmt, 1, name, strlen(name), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getCommandStmt, 2, cls, strlen(cls), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_step(getCommandStmt) == SQLITE_ROW)) {
		const guint8 *bytes = (const guint8*) sqlite3_column_blob(getCommandStmt, 0);
		*count = sqlite3_column_bytes(getCommandStmt, 0);
		memcpy(buffer, bytes, *count);
		return TRUE;
	}

	return FALSE;
}

gboolean database_get_command_by_id(gint32 id, guint8 *buffer, size_t *count) {

	if (getCommandByIdStmt == NULL) {
		const gchar *text = "SELECT \"Command\" FROM \"Command\" WHERE \"ID\"=:id";
		int ret = sqlite3_prepare_v2(db, text, strlen(text), &getCommandByIdStmt, NULL);
		if (ret != SQLITE_OK) {
			sqlite3_finalize(getCommandByIdStmt);
			getCommandByIdStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(getCommandByIdStmt) == SQLITE_OK)
			&& (sqlite3_bind_int(getCommandByIdStmt, 1, id) == SQLITE_OK)
			&& (sqlite3_step(getCommandByIdStmt) == SQLITE_ROW)) {
		const guint8 *bytes = (const guint8*) sqlite3_column_blob(getCommandByIdStmt, 0);
		*count = sqlite3_column_bytes(getCommandByIdStmt, 0);
		memcpy(buffer, bytes, *count);
		return TRUE;
	}

	return FALSE;
}
