#include "database.h"
#include "register.h"
#include "sysdb.h"

Database * database_new(const gchar *filePath) {
	gchar password[128] = { 0 };
	Database *obj = (Database*) g_malloc(sizeof(Database));
	obj->tcCatalog = NULL;
	obj->ldCatalog = NULL;
	obj->cmdCatalog = NULL;
	obj->handle = NULL;
	obj->getTextStmt = NULL;
	obj->getTroubleCodeStmt = NULL;
	obj->getLiveDataStmt = NULL;
	obj->getCommandByIDStmt = NULL;
	obj->getCommandStmt = NULL;

	sqlite3_enable_shared_cache(1);

	if (sqlite3_open_v2(filePath, &obj->handle, SQLITE_OPEN_READONLY,
			NULL) != SQLITE_OK) {
		obj->handle = NULL;
		g_free(obj);
		return NULL;
	}

	register_decrypt(DatabasePassword, password);
	sqlite3_busy_timeout(obj->handle, 5000);
	sqlite3_key(obj->handle, password, strlen(password));

	obj->tcCatalog = g_string_new("");
	obj->ldCatalog = g_string_new("");
	obj->cmdCatalog = g_string_new("");

	return obj;
}

void database_dispose(Database *obj) {
	sqlite3_stmt *stmt = NULL;
	g_return_if_fail(obj != NULL);

	while ((stmt = sqlite3_next_stmt(obj->handle, 0)) != NULL) {
		sqlite3_finalize(stmt);
	}

	sqlite3_close(obj->handle);
	g_string_free(obj->tcCatalog, TRUE);
	g_string_free(obj->ldCatalog, TRUE);
	g_string_free(obj->cmdCatalog, TRUE);

	g_free(obj);
}

void database_set_tc_catalog(Database *obj, const gchar *catalog) {
	g_return_if_fail(obj != NULL);
	g_string_truncate(obj->tcCatalog, 0);
	g_string_append(obj->tcCatalog, catalog);
}

void database_set_ld_catalog(Database *obj, const gchar *catalog) {
	g_return_if_fail(obj != NULL);
	g_string_truncate(obj->ldCatalog, 0);
	g_string_append(obj->ldCatalog, catalog);
}

void database_set_cmd_catalog(Database *obj, const gchar *catalog) {
	g_return_if_fail(obj != NULL);
	g_string_truncate(obj->cmdCatalog, 0);
	g_string_append(obj->cmdCatalog, catalog);
}

gboolean database_get_text(Database *obj, const gchar *name, gunichar2 *text) {
	long items_read = 0;
	long items_written = 0;
	gunichar2 *temp = NULL;
	g_return_val_if_fail(obj != NULL, FALSE);

	temp = g_utf8_to_utf16(name, g_utf8_strlen(name, -1), &items_read,
			&items_written, NULL);
	memcpy(text, temp, sizeof(gunichar2) * (items_written + 1));
	g_free(temp);

	if (obj->getTextStmt == NULL) {
		gchar lang[128];
		int ret = 0;
		GString *temp = g_string_new("SELECT Content FROM [Text");
		register_decrypt(Language, lang);

		temp = g_string_append(temp, lang);
		temp = g_string_append(temp, "] WHERE Name=:name");

		ret = sqlite3_prepare_v2(obj->handle, temp->str, temp->len,
				&obj->getTextStmt, NULL);
		g_string_free(temp, TRUE);
		if (ret != SQLITE_OK) {
			sqlite3_finalize(obj->getTextStmt);
			obj->getTextStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(obj->getTextStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(obj->getTextStmt, 1, name, strlen(name),
					SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_step(obj->getTextStmt) == SQLITE_ROW)) {
		const gunichar2 *temp = (const gunichar2 *) sqlite3_column_text16(
				obj->getTextStmt, 0);
		if (temp != NULL)
			memcpy(text, temp,
					sqlite3_column_bytes16(obj->getTextStmt, 0)
							+ sizeof(gunichar2));
	}

	return TRUE;
}

gboolean database_get_trouble_code(Database *obj, const gchar *code,
		gunichar2 *text) {
	long items_read = 0;
	long items_written = 0;
	gunichar2 *temp = NULL;
	g_return_val_if_fail(obj != NULL, FALSE);
	temp = g_utf8_to_utf16(code, g_utf8_strlen(code, -1), &items_read,
			&items_written, NULL);
	memcpy(text, temp, sizeof(gunichar2) * (items_written + 1));
	g_free(temp);

	if (obj->getTroubleCodeStmt == NULL) {
		char lang[128];
		int ret = 0;
		GString *temp = g_string_new("SELECT Content FROM [TroubleCode");

		register_decrypt(Language, lang);
		temp = g_string_append(temp, lang);
		temp = g_string_append(temp, "] WHERE Code=:code AND Catalog=:catalog");

		ret = sqlite3_prepare_v2(obj->handle, temp->str, temp->len,
				&obj->getTroubleCodeStmt, NULL);
		g_string_free(temp, TRUE);
		if (ret != SQLITE_OK) {
			sqlite3_finalize(obj->getTroubleCodeStmt);
			obj->getTroubleCodeStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(obj->getTroubleCodeStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(obj->getTroubleCodeStmt, 1, code,
					strlen(code), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(obj->getTroubleCodeStmt, 2,
					obj->tcCatalog->str, obj->tcCatalog->len, SQLITE_STATIC)
					== SQLITE_OK)) {
		int rc = sqlite3_step(obj->getTroubleCodeStmt);
		if (rc == SQLITE_ROW) {
			const gunichar2* temp = (const gunichar2*) sqlite3_column_text16(
					obj->getTroubleCodeStmt, 0);
			if (temp) {
				memcpy(text, temp,
						sqlite3_column_bytes16(obj->getTroubleCodeStmt, 0)
								+ sizeof(gunichar2));
			} else {
				sys_db_get_text("Undefined", text);
			}
		} else if (rc == SQLITE_DONE) {
			sys_db_get_text("Undefined", text);
		}
	}

	return TRUE;
}

gboolean database_live_data_prepare(Database *obj) {
	g_return_val_if_fail(obj != NULL, FALSE);

	if (obj->getLiveDataStmt == NULL) {
		gchar lang[128];
		GString *temp =
				g_string_new(
						"SELECT ShortName, Content, Unit, DefaultValue, CommandID, AlgorithmID FROM [LiveData");
		int ret = 0;

		register_decrypt(Language, lang);
		temp = g_string_append(temp, lang);
		temp = g_string_append(temp, "] WHERE Catalog=:catalog");

		ret = sqlite3_prepare_v2(obj->handle, temp->str, temp->len,
				&obj->getLiveDataStmt, NULL);
		g_string_free(temp, TRUE);

		if (ret != SQLITE_OK) {
			sqlite3_finalize(obj->getLiveDataStmt);
			obj->getLiveDataStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(obj->getLiveDataStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(obj->getLiveDataStmt, 1, obj->ldCatalog->str,
					obj->ldCatalog->len, SQLITE_STATIC) == SQLITE_OK)) {
		return TRUE;
	}

	return FALSE;
}

gboolean database_live_data_next(Database *obj, gunichar2 *shortName,
		gunichar2 *content, gunichar2 *unit, gunichar2 *defaultValue,
		gunichar2 *value, gint32 *cmdID)
{
	g_return_val_if_fail(obj != NULL, FALSE);

	if (sqlite3_step(obj->getLiveDataStmt) == SQLITE_ROW)
	{
		const gunichar2 *shortNameTemp = (const gunichar2 *)sqlite3_column_text16(obj->getLiveDataStmt, 0);
		const gunichar2 *contentTemp = (const gunichar2 *)sqlite3_column_text16(obj->getLiveDataStmt, 1);
		const gunichar2 *unitTemp = (const gunichar2 *)sqlite3_column_text16(obj->getLiveDataStmt, 2);
		const gunichar2 *defaultValueTemp = (const gunichar2 *)sqlite3_column_text16(obj->getLiveDataStmt, 3);
		*cmdID = sqlite3_column_int(obj->getLiveDataStmt, 4);
		memcpy(shortName, shortNameTemp, sqlite3_column_bytes16(obj->getLiveDataStmt, 0) + sizeof(gunichar2));
		memcpy(content, contentTemp, sqlite3_column_bytes16(obj->getLiveDataStmt, 1) + sizeof(gunichar2));
		memcpy(unit, unitTemp, sqlite3_column_bytes16(obj->getLiveDataStmt, 2) + sizeof(gunichar2));
		memcpy(defaultValue, defaultValueTemp, sqlite3_column_bytes16(obj->getLiveDataStmt, 3) + sizeof(gunichar2));
		value[0] = 0;
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

gboolean database_get_command(Database *obj, const gchar *name, guint8 *buffer,
		size_t *count) {
	g_return_val_if_fail(obj != NULL, FALSE);

	if (obj->getCommandStmt == NULL) {
		static const char *temp =
				"SELECT Command FROM [Command] WHERE Name=:name AND Catalog=:catalog";
		int ret;
		ret = sqlite3_prepare_v2(obj->handle, temp, strlen(temp),
				&obj->getCommandStmt, NULL);
		if (ret != SQLITE_OK) {
			sqlite3_finalize(obj->getCommandStmt);
			obj->getCommandStmt = NULL;
			return FALSE;
		}
	}
	if ((sqlite3_reset(obj->getCommandStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(obj->getCommandStmt, 1, name, strlen(name),
					SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(obj->getCommandStmt, 2, obj->cmdCatalog->str,
					obj->cmdCatalog->len, SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_step(obj->getCommandStmt) == SQLITE_ROW)) {
		const guint8 *bytes = (const guint8*) sqlite3_column_blob(
				obj->getCommandStmt, 0);
		*count = sqlite3_column_bytes(obj->getCommandStmt, 0);
		memcpy(buffer, bytes, *count);
		return TRUE;
	}

	return FALSE;
}

gboolean database_get_command_by_id(Database *obj, gint32 id, guint8 *buffer,
		size_t *count) {
	g_return_val_if_fail(obj != NULL, FALSE);

	if (obj->getCommandByIDStmt == NULL) {
		static const gchar *text = "SELECT Command FROM [Command] WHERE ID=:id";
		int ret = sqlite3_prepare_v2(obj->handle, text, strlen(text),
				&obj->getCommandByIDStmt, NULL);
		if (ret != SQLITE_OK) {
			sqlite3_finalize(obj->getCommandByIDStmt);
			obj->getCommandByIDStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(obj->getCommandByIDStmt) == SQLITE_OK)
			&& (sqlite3_bind_int(obj->getCommandByIDStmt, 1, id) == SQLITE_OK)
			&& (sqlite3_step(obj->getCommandByIDStmt) == SQLITE_ROW)) {
		const guint8 *bytes = (const guint8*) sqlite3_column_blob(
				obj->getCommandByIDStmt, 0);
		*count = sqlite3_column_bytes(obj->getCommandByIDStmt, 0);
		memcpy(buffer, bytes, *count);
		return TRUE;
	}

	return FALSE;
}
