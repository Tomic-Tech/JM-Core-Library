#include "sysdb.h"
#include "register.h"

static gchar dbPath[1024];
static sqlite3 *db = NULL;
static sqlite3_stmt *getTextStmt = NULL;

void sys_db_init(const gchar *path)
{
	int ret;
	gchar password[128] = {0};
	gchar lang[128];
	GString *temp = NULL;
	g_stpcpy(dbPath, path);
	g_stpcpy(dbPath + strlen(path), "sys.db");

	ret = sqlite3_open_v2(dbPath, &db, SQLITE_OPEN_READONLY, NULL);
	if (ret != SQLITE_OK) {
		db = NULL;
	}

	if (register_decrypt(DatabasePassword, password))
		sqlite3_key(db, password, strlen(password));
	else
		return;

	if (!register_decrypt(Language, lang))
		return;
	temp = g_string_new("SELECT Content FROM[Text");
	temp = g_string_append(temp, lang);
	temp = g_string_append(temp, "] WHERE Name=:name");

	ret = sqlite3_prepare_v2(db, temp->str, temp->len, &getTextStmt, NULL);
	g_string_free(temp, TRUE);
}

void sys_db_dispose(void)
{
	sqlite3_stmt *stmt = NULL;
	while ((stmt = sqlite3_next_stmt(db, 0)) != NULL) {
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);
	db = NULL;
}

void sys_db_get_text(const gchar *name, gunichar2 *text)
{
	long items_read = 0;
	long items_written = 0;
	gunichar2 *temp = g_utf8_to_utf16(name, g_utf8_strlen(name, -1), &items_read, &items_written, NULL);
	memcpy(text, temp, sizeof(gunichar2) * (items_written + 1));
	g_free(temp);

	if (sqlite3_reset(getTextStmt) == SQLITE_OK &&
		(sqlite3_bind_text(getTextStmt, 1, name, strlen(name), SQLITE_STATIC) == SQLITE_OK) &&
		(sqlite3_step(getTextStmt) == SQLITE_ROW)) {
			const gunichar2 *temp = (const gunichar2 *)sqlite3_column_text16(getTextStmt, 0);
			int bytes = sqlite3_column_bytes16(getTextStmt, 0);
			if (temp != NULL) {
				memcpy(text, temp, bytes + sizeof(gunichar2));
			}
	}
}
