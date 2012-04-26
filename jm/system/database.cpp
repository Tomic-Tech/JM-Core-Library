#include "database.hpp"

namespace JM
{
namespace System
{
Database::Database()
    : _path()
    , _db(NULL)
    , _getTextStmt(NULL)
{
    sqlite3_enable_shared_cache(1);
}

Database::~Database()
{
    if (_db == NULL)
        return;

    sqlite3_stmt *stmt = NULL;
    while ((stmt = sqlite3_next_stmt(_db, 0)) != NULL)
    {
        sqlite3_finalize(stmt);
    }
    sqlite3_close(_db);
    _db = NULL;
}
Database& Database::inst()
{
    static Database instance;
    return instance;
}

void Database::setPath(const std::string &path)
{
    _path = path;
}

std::string Database::text(const std::string &name)
{
    std::string result(name);

    if (_db == NULL)
    {
        int ret;
        std::string dbPath = _path;
        dbPath.append("sys.db");
        std::string dbPW = Register::inst().decrypt(Register::DB_PW);

        ret = sqlite3_open_v2(dbPath.c_str(), &_db, SQLITE_OPEN_READONLY, NULL);
        if (ret != SQLITE_OK)
        {
            _db = NULL;
            return result;
        }
        sqlite3_key(_db, dbPW.c_str(), dbPW.length());
    }

    if (_getTextStmt == NULL)
    {
        std::string lang = Register::inst().decrypt(Register::LANG);
        int ret = 0;
        std::string temp = "SELECT Content FROM[Text";
        temp += lang;
        temp += "] WHERE Name=:name";
        ret = sqlite3_prepare_v2(_db, temp.c_str(), temp.length(), &_getTextStmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_getTextStmt);
            _getTextStmt = NULL;
            return result;
        }
    }

    if ((sqlite3_reset(_getTextStmt) == SQLITE_OK) &&
            (sqlite3_bind_text(_getTextStmt, 1, name.c_str(), name.length(), SQLITE_STATIC) == SQLITE_OK) &&
            (sqlite3_step(_getTextStmt) == SQLITE_ROW))
    {
        const char* temp = (const char*)sqlite3_column_text(_getTextStmt, 0);
        if (temp != NULL)
            result = temp;
    }

    return result;
}
}
}