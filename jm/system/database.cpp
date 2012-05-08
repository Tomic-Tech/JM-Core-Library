#include "database.h"
#include <sqlite3.h>
#include "register.h"

namespace JM
{
namespace System
{
class DatabasePrivate
{
    friend class Database;
    DatabasePrivate()
        : path()
        , db(NULL)
        , getTextStmt(NULL)
    {

    }
    ~DatabasePrivate()
    {
        if (db == NULL)
            return;

        sqlite3_stmt *stmt = NULL;
        while ((stmt = sqlite3_next_stmt(db, 0)) != NULL)
        {
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
        db = NULL;
    }
    std::string path;
    sqlite3 *db;
    sqlite3_stmt *getTextStmt;
};
Database::Database()
    : _priv(new DatabasePrivate())
{
    sqlite3_enable_shared_cache(1);
}

Database::~Database()
{
    delete _priv;
}
Database& Database::inst()
{
    static Database instance;
    return instance;
}

void Database::set_path(const std::string &path)
{
    _priv->path = path;
}

std::string Database::text(const std::string &name)
{
    std::string result(name);

    if (_priv->db == NULL)
    {
        int ret;
        std::string dbPath = _priv->path;
        dbPath.append("sys.db");
        std::string dbPW = Register::inst().decrypt(Register::DB_PW);

        ret = sqlite3_open_v2(dbPath.c_str(), &_priv->db, SQLITE_OPEN_READONLY, NULL);
        if (ret != SQLITE_OK)
        {
            _priv->db = NULL;
            return result;
        }
        sqlite3_key(_priv->db, dbPW.c_str(), dbPW.length());
    }

    if (_priv->getTextStmt == NULL)
    {
        std::string lang = Register::inst().decrypt(Register::LANG);
        int ret = 0;
        std::string temp = "SELECT Content FROM[Text";
        temp += lang;
        temp += "] WHERE Name=:name";
        ret = sqlite3_prepare_v2(_priv->db, temp.c_str(), temp.length(), &_priv->getTextStmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_priv->getTextStmt);
            _priv->getTextStmt = NULL;
            return result;
        }
    }

    if ((sqlite3_reset(_priv->getTextStmt) == SQLITE_OK) &&
            (sqlite3_bind_text(_priv->getTextStmt, 1, name.c_str(), name.length(), SQLITE_STATIC) == SQLITE_OK) &&
            (sqlite3_step(_priv->getTextStmt) == SQLITE_ROW))
    {
        const char* temp = (const char*)sqlite3_column_text(_priv->getTextStmt, 0);
        if (temp != NULL)
            result = temp;
    }

    return result;
}
}
}