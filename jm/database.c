#include "database.h"
#include "register.h"

static char dbPath[1024];
static sqlite3 *db = NULL;
static sqlite3_stmt *getTextStmt = NULL;
static sqlite3_stmt *getTroubleCodeStmt = NULL;
static sqlite3_stmt *getLiveDataStmt = NULL;
static sqlite3_stmt *getCommandByIdStmt = NULL;
static sqlite3_stmt *getCommandStmt = NULL;
static char language[128] = {0};
static GString *liveDataClass = NULL;

void database_init(const char *path)
{
	int ret;
	char password[128] = { 0 };

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
	while ((stmt = sqlite3_next_stmt(db, 0)) != NULL)
	{
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);
	db = NULL;
	g_string_free(liveDataClass, TRUE);
}

gboolean database_get_text(const char *name, const char *cls, wchar_t *text)
{
	long items_read = 0;
	long items_written = 0;
	wchar_t *temp = NULL;

	temp = (wchar_t*)g_utf8_to_utf16(name, g_utf8_strlen(name, -1), &items_read, &items_written, NULL);
	memcpy(text, temp, sizeof(wchar_t) * (items_written + 1));
	g_free(temp);

	if (getTextStmt == NULL)
	{
		int ret = 0;
		char *temp = "SELECT Content FROM [Text] WHERE [Name]=:name AND [Language]=:language AND [Class]=:class";

		//temp = g_string_append(temp, lang);
		//temp = g_string_append(temp, "] WHERE Name=:name");

		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getTextStmt, NULL);
		if (ret != SQLITE_OK)
		{
			sqlite3_finalize(getTextStmt);
			getTextStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(getTextStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(getTextStmt, 1, name, strlen(name), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTextStmt, 2, language, strlen(language), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTextStmt, 3, cls, strlen(cls), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_step(getTextStmt) == SQLITE_ROW))
	{
		const wchar_t *temp = (const wchar_t *) sqlite3_column_text16(getTextStmt, 0);
		if (temp != NULL)
			memcpy(text, temp, sqlite3_column_bytes16(getTextStmt, 0)+ sizeof(wchar_t));
	}

	return TRUE;
}

gboolean database_get_trouble_code(const char *code, const char *cls, wchar_t *content, wchar_t *description)
{
	long items_read = 0;
	long items_written = 0;
	wchar_t *temp = NULL;
	temp = (wchar_t*)g_utf8_to_utf16(code, g_utf8_strlen(code, -1), &items_read, &items_written, NULL);
	memcpy(content, temp, sizeof(wchar_t) * (items_written + 1));
	g_free(temp);

	if (getTroubleCodeStmt == NULL)
	{
		int ret = 0;
		char *temp = "SELECT [Content], [Description] FROM [TroubleCode] WHERE [Code]=:code AND [Language]=:language AND [Class]=:class";

		//temp = g_string_append(temp, lang);
		//temp = g_string_append(temp, "] WHERE Code=:code AND Catalog=:catalog");

		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getTroubleCodeStmt, NULL);
		if (ret != SQLITE_OK)
		{
			sqlite3_finalize(getTroubleCodeStmt);
			getTroubleCodeStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(getTroubleCodeStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(getTroubleCodeStmt, 1, code, strlen(code), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTroubleCodeStmt, 2, language, strlen(language), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getTroubleCodeStmt, 3, cls, strlen(cls), SQLITE_STATIC) == SQLITE_OK))
	{
		int rc = sqlite3_step(getTroubleCodeStmt);
		if (rc == SQLITE_ROW)
		{
			const wchar_t* temp = (const wchar_t*) sqlite3_column_text16(getTroubleCodeStmt, 0);
			if (temp)
			{
				memcpy(content, temp, sqlite3_column_bytes16(getTroubleCodeStmt, 0) + sizeof(wchar_t));
			}
			else
			{
				database_get_text("Undefined", "System", content);
			}
			temp = (const wchar_t*) sqlite3_column_text16(getTroubleCodeStmt, 1);
			if (temp)
			{
				memcpy(description, temp, sqlite3_column_bytes16(getTroubleCodeStmt, 1) + sizeof(wchar_t));
			}
			else
			{
				database_get_text("Undefined", "System", description);
			}
		}
		else if (rc == SQLITE_DONE)
		{
			database_get_text("Undefined", "System", content);
			database_get_text("Undefined", "System", description);
		}
	}

	return TRUE;
}

gboolean database_live_data_prepare(const char *cls)
{
	if (getLiveDataStmt == NULL)
	{
		char *temp = "SELECT [ShortName], [Content], [Unit], [DefaultValue], [CommandName], [CommandClass], [Description], [Index] FROM [LiveData] WHERE [Language]=:language AND [Class]=:class";
		int ret = 0;
		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getLiveDataStmt, NULL);

		if (ret != SQLITE_OK)
		{
			sqlite3_finalize(getLiveDataStmt);
			getLiveDataStmt = NULL;
			return FALSE;
		}
	}

	liveDataClass = g_string_set_size(liveDataClass, 0);
	liveDataClass = g_string_append(liveDataClass, cls);

	if ((sqlite3_reset(getLiveDataStmt) == SQLITE_OK)
		&& (sqlite3_bind_text(getLiveDataStmt, 1, language, strlen(language), SQLITE_STATIC) == SQLITE_OK)
		&& (sqlite3_bind_text(getLiveDataStmt, 2, liveDataClass->str, liveDataClass->len, SQLITE_STATIC) == SQLITE_OK)
		)
	{
		return TRUE;
	}


	return FALSE;
}

gboolean database_live_data_next(wchar_t *shortName, 
								 wchar_t *content, 
								 wchar_t *unit, 
								 wchar_t *defaultValue, 
								 wchar_t *cmdName,
								 wchar_t *cmdClass,
								 wchar_t* description,
								 int32_t *index)
{
	if (sqlite3_step(getLiveDataStmt) == SQLITE_ROW)
	{
		wchar_t *temp = (wchar_t*)sqlite3_column_text16(getLiveDataStmt, 0);
		if (temp != NULL)
		{
			memcpy(shortName, temp, sqlite3_column_bytes16(getLiveDataStmt, 0) + sizeof(wchar_t));
		}
		else
		{
			shortName[0] = 0;
		}

		temp = (wchar_t*)sqlite3_column_text16(getLiveDataStmt, 1);
		if (temp != NULL)
		{
			memcpy(content, temp, sqlite3_column_bytes16(getLiveDataStmt, 1) + sizeof(wchar_t));
		}
		else
		{
			content[0] = 0;
		}
		
		temp = (wchar_t*)sqlite3_column_text16(getLiveDataStmt, 2);
		if (temp != NULL)
		{
			memcpy (unit, temp, sqlite3_column_bytes16(getLiveDataStmt, 2) + sizeof(wchar_t));
		}
		else
		{
			unit[0] = 0;
		}
		
		temp = (wchar_t*)sqlite3_column_text16(getLiveDataStmt, 3);
		if (temp != NULL)
		{
			memcpy (defaultValue, temp, sqlite3_column_bytes16(getLiveDataStmt, 3) + sizeof(wchar_t));
		}
		else
		{
			defaultValue[0] = 0;
		}

		temp = (wchar_t*)sqlite3_column_text16(getLiveDataStmt, 4);
		if (temp != NULL)
		{
			memcpy (cmdName, temp, sqlite3_column_bytes16(getLiveDataStmt, 4) + sizeof(wchar_t));
		}
		else
		{
			cmdName[0] = 0;
		}

		temp = (wchar_t*)sqlite3_column_text16(getLiveDataStmt, 5);
		if (temp != NULL)
		{
			memcpy (cmdClass, temp, sqlite3_column_bytes16(getLiveDataStmt, 5) + sizeof(wchar_t));
		}
		else
		{
			cmdClass[0] = 0;
		}

		temp = (wchar_t*)sqlite3_column_text16(getLiveDataStmt, 6);
		if (temp != NULL)
		{
			memcpy (description, temp, sqlite3_column_bytes16(getLiveDataStmt, 6) + sizeof(wchar_t));
		}
		else
		{
			description[0] = 0;
		}
		*index = sqlite3_column_int(getLiveDataStmt, 7);

		return TRUE;
	}
	return FALSE;
}

gboolean database_get_command(const char *name, const char *cls, uint8_t *buffer, size_t *count)
{
	if (getCommandStmt == NULL)
	{
		const char *temp = "SELECT \"Command\" FROM \"Command\" WHERE \"Name\"=:name AND \"Class\"=:catalog";
		int ret;
		ret = sqlite3_prepare_v2(db, temp, strlen(temp), &getCommandStmt, NULL);
		if (ret != SQLITE_OK)
		{
			sqlite3_finalize(getCommandStmt);
			getCommandStmt = NULL;
			return FALSE;
		}
	}
	if ((sqlite3_reset(getCommandStmt) == SQLITE_OK)
			&& (sqlite3_bind_text(getCommandStmt, 1, name, strlen(name), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_bind_text(getCommandStmt, 2, cls, strlen(cls), SQLITE_STATIC) == SQLITE_OK)
			&& (sqlite3_step(getCommandStmt) == SQLITE_ROW))
	{
		const uint8_t *bytes = (const uint8_t*) sqlite3_column_blob(getCommandStmt, 0);
		*count = sqlite3_column_bytes(getCommandStmt, 0);
		memcpy(buffer, bytes, *count);
		return TRUE;
	}

	return FALSE;
}

gboolean database_get_command_by_id(int32_t id, uint8_t *buffer, size_t *count)
{

	if (getCommandByIdStmt == NULL)
	{
		const char *text = "SELECT \"Command\" FROM \"Command\" WHERE \"ID\"=:id";
		int ret = sqlite3_prepare_v2(db, text, strlen(text), &getCommandByIdStmt, NULL);
		if (ret != SQLITE_OK)
		{
			sqlite3_finalize(getCommandByIdStmt);
			getCommandByIdStmt = NULL;
			return FALSE;
		}
	}

	if ((sqlite3_reset(getCommandByIdStmt) == SQLITE_OK)
			&& (sqlite3_bind_int(getCommandByIdStmt, 1, id) == SQLITE_OK)
			&& (sqlite3_step(getCommandByIdStmt) == SQLITE_ROW))
	{
		const uint8_t *bytes = (const uint8_t*) sqlite3_column_blob(getCommandByIdStmt, 0);
		*count = sqlite3_column_bytes(getCommandByIdStmt, 0);
		memcpy(buffer, bytes, *count);
		return TRUE;
	}

	return FALSE;
}
