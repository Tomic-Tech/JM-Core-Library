#include "database.h"
#include <sqlite3.h>
#include "../system/register.h"
#include "../system/database.h"

namespace JM
{
namespace Vehicle
{
class DatabasePrivate
{
    friend class Database;

public:
    DatabasePrivate()
        : tcCatalog()
        , ldCatalog()
        , cmdCatalog()
        , handle(NULL)
        , getTextStmt(NULL)
        , get_trouble_codeStmt(NULL)
        , get_commandStmt(NULL)
        , get_commandByIDStmt(NULL)
        , get_live_dataStmt(NULL)
    {

    }
    std::string tcCatalog;
    std::string ldCatalog;
    std::string cmdCatalog;
    sqlite3 *handle;
    sqlite3_stmt *getTextStmt;
    sqlite3_stmt *get_trouble_codeStmt;
    sqlite3_stmt *get_commandStmt;
    sqlite3_stmt *get_commandByIDStmt;
    sqlite3_stmt *get_live_dataStmt;
};

Database::Database()
    : _priv(new DatabasePrivate())
{
    sqlite3_enable_shared_cache(1);
}

Database::~Database()
{
    close();
    delete _priv;
}

Database& Database::inst()
{
    static Database instance;
    return instance;
}

bool Database::open(const std::string &filePath, const std::string &password)
{
    close();

    if (sqlite3_open_v2(filePath.c_str(), &_priv->handle, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
    {
        _priv->handle = NULL;
        return false;
    }

    sqlite3_busy_timeout(_priv->handle, 5000);
    sqlite3_key(_priv->handle, password.c_str(), password.size());
    return true;
}

bool Database::close()
{
    int ret;
    sqlite3_stmt *stmt = NULL;
    _priv->getTextStmt = NULL;
    _priv->get_trouble_codeStmt = NULL;
    _priv->get_commandStmt = NULL;
    _priv->get_commandByIDStmt = NULL;
    _priv->get_live_dataStmt = NULL;

    if (_priv->handle == NULL)
    {
        return true;
    }
    while ((stmt = sqlite3_next_stmt(_priv->handle, 0)) != NULL)
    {
        ret = sqlite3_finalize(stmt);
        if (ret != SQLITE_OK)
        {
            return false;
        }
    }
    ret = sqlite3_close(_priv->handle);
    if (ret != SQLITE_OK)
    {
        return false;
    }
    _priv->handle = NULL;
    return true;
}

bool Database::is_open()
{
    return _priv->handle != NULL ? true : false;
}

void Database::set_tc_catalog(const std::string &catalog)
{
    _priv->tcCatalog = catalog;
}

void Database::set_ld_catalog(const std::string &catalog)
{
    _priv->ldCatalog = catalog;
}

void Database::set_cmd_catalog(const std::string &catalog)
{
    _priv->cmdCatalog = catalog;
}

std::string Database::get_text(const std::string &name)
{
    std::string result(name);
    if (_priv->getTextStmt == NULL)
    {
        std::string lang = System::Register::inst().decrypt(System::Register::LANG);
        int ret = 0;
        std::string text = "SELECT Content FROM [Text";
        text += lang;
        text += "] WHERE Name=:name";

        ret = sqlite3_prepare_v2(_priv->handle, text.c_str(), text.size(), &_priv->getTextStmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_priv->getTextStmt);
            _priv->getTextStmt = NULL;
            return result;
        }
    }

    if ((sqlite3_reset(_priv->getTextStmt) == SQLITE_OK) &&
            (sqlite3_bind_text(_priv->getTextStmt, 1, name.c_str(), name.size(), SQLITE_STATIC) == SQLITE_OK) &&
            (sqlite3_step(_priv->getTextStmt) == SQLITE_ROW))
    {
        const char* text = (const char*)sqlite3_column_text(_priv->getTextStmt, 0);
        if (text != NULL)
            result = text;
    }

    return result;
}

std::string Database::get_trouble_code(const std::string &code)
{
    std::string result(code);

    if (_priv->get_trouble_codeStmt == NULL)
    {
        std::string lang = System::Register::inst().decrypt(System::Register::LANG);
        int ret = 0;
        std::string text = "SELECT Content FROM [TroubleCode";
        text += lang;
        text += "] WHERE Code=:code AND Catalog=:catalog";

        ret = sqlite3_prepare_v2(_priv->handle, text.c_str(), text.size(), &_priv->get_trouble_codeStmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_priv->get_trouble_codeStmt);
            _priv->get_trouble_codeStmt = NULL;
            return result;
        }
    }

    if ((sqlite3_reset(_priv->get_trouble_codeStmt) == SQLITE_OK) &&
            (sqlite3_bind_text(_priv->get_trouble_codeStmt, 1, code.c_str(), code.size(), SQLITE_STATIC) == SQLITE_OK) &&
            (sqlite3_bind_text(_priv->get_trouble_codeStmt, 2, _priv->tcCatalog.c_str(), _priv->tcCatalog.size(), SQLITE_STATIC) == SQLITE_OK))
    {
        int rc = sqlite3_step(_priv->get_trouble_codeStmt);
        if (rc == SQLITE_ROW)
        {
            const char* text = (const char*)sqlite3_column_text(_priv->get_trouble_codeStmt, 0);
            result = text ? text : System::Database::inst().text("Undefined");
        }
        else if (rc == SQLITE_DONE)
        {
            result = System::Database::inst().text("Undefined");
        }
    }

    return result;
}

Diag::LiveDataVectorPtr Database::get_live_data(bool showed)
{
    if (_priv->get_live_dataStmt == NULL)
    {
        std::string lang = System::Register::inst().decrypt(System::Register::LANG);
        std::string text = "SELECT ShortName, Content, Unit, DefaultValue, CommandID, AlgorithmID FROM [LiveData";
        text += lang;
        text += "] WHERE Catalog=:catalog";

        int ret = sqlite3_prepare_v2(_priv->handle, text.c_str(), text.size(), &_priv->get_live_dataStmt, NULL);

        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_priv->get_live_dataStmt);
            _priv->get_live_dataStmt = NULL;
            return boost::make_shared<Diag::LiveDataVector>();
        }
    }

    if ((sqlite3_reset(_priv->get_live_dataStmt) == SQLITE_OK) &&
            (sqlite3_bind_text(_priv->get_live_dataStmt, 1, _priv->ldCatalog.c_str(), _priv->ldCatalog.size(), SQLITE_STATIC) == SQLITE_OK))
    {
        Diag::LiveDataVectorPtr result = boost::make_shared<Diag::LiveDataVector>();

        while (sqlite3_step(_priv->get_live_dataStmt) == SQLITE_ROW)
        {
            const char *shortName = (const char*)sqlite3_column_text(_priv->get_live_dataStmt, 0);
            const char *content = (const char*)sqlite3_column_text(_priv->get_live_dataStmt, 1);
            const char *unit = (const char*)sqlite3_column_text(_priv->get_live_dataStmt, 2);
            const char *defaultValue = (const char*)sqlite3_column_text(_priv->get_live_dataStmt, 3);
            boost::int32_t id = sqlite3_column_int(_priv->get_live_dataStmt, 4);
            bool enabled = true;

            Diag::LiveDataPtr ld = boost::make_shared<Diag::LiveData>(shortName ? shortName : "",
                             content ? content : "",
                             unit ? unit : "",
                             defaultValue ? defaultValue : "",
                             id,
                             showed);
            result->push_back(ld);
        }
        return result;
    }

    return boost::make_shared<Diag::LiveDataVector>();
}

const boost::uint8_t * Database::get_command(const std::string &name, std::size_t &count)
{
    if (_priv->get_commandStmt == NULL)
    {
        static const char *text = "SELECT Command FROM [Command] WHERE Name=:name AND Catalog=:catalog";
        int ret;
        ret = sqlite3_prepare_v2(_priv->handle, text, strlen(text), &_priv->get_commandStmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_priv->get_commandStmt);
            _priv->get_commandStmt = NULL;
            return NULL;
        }
    }
    if ((sqlite3_reset(_priv->get_commandStmt) == SQLITE_OK) &&
        (sqlite3_bind_text(_priv->get_commandStmt, 1, name.c_str(), name.size(), SQLITE_STATIC) == SQLITE_OK) &&
        (sqlite3_bind_text(_priv->get_commandStmt, 2, _priv->cmdCatalog.c_str(), _priv->cmdCatalog.size(), SQLITE_STATIC) == SQLITE_OK) &&
        (sqlite3_step(_priv->get_commandStmt) == SQLITE_ROW))
    {
        const boost::uint8_t *bytes = (const boost::uint8_t*)sqlite3_column_blob(_priv->get_commandStmt, 0);
        count = sqlite3_column_bytes(_priv->get_commandStmt, 0);
        return bytes;
    }
    return NULL;
}

const boost::uint8_t * Database::get_command(boost::int32_t id, std::size_t &count)
{
    if (_priv->get_commandByIDStmt == NULL)
    {
        static const char *text = "SELECT Command FROM [Command] WHERE ID=:id";
        int ret = sqlite3_prepare_v2(_priv->handle, text, strlen(text), &_priv->get_commandByIDStmt, NULL);
        if (ret != SQLITE_OK)
        {
            sqlite3_finalize(_priv->get_commandByIDStmt);
            _priv->get_commandByIDStmt = NULL;
            return 0;
        }
    }

    if ((sqlite3_reset(_priv->get_commandByIDStmt) == SQLITE_OK) &&
        (sqlite3_bind_int(_priv->get_commandByIDStmt, 1, id) == SQLITE_OK) &&
        (sqlite3_step(_priv->get_commandByIDStmt) == SQLITE_ROW))
    {
        const boost::uint8_t *bytes = (const boost::uint8_t*) sqlite3_column_blob(_priv->get_commandByIDStmt, 0);
        count = sqlite3_column_bytes(_priv->get_commandByIDStmt, 0);
        return bytes;
    }
    return NULL;
}
}
}