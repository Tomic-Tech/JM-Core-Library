#ifndef __JM_DB_HPP__
#define __JM_DB_HPP__

#include <sqlite/sqlite3.h>
#include <string>
#include <boost/asio/buffer.hpp>
#include "jmauth.hpp"
#include "jmsys.hpp"
#include "jmldvector.hpp"

namespace JM
{
    class Database
    {
    public:
        static Database& inst();
        bool open(const std::string &filePath, const std::string &password);
        bool close();
        bool isOpen();
        void setTCCatalog(const std::string &catalog);
        void setLDCatalog(const std::string &catalog);
        void setCMDCatalog(const std::string &catalog);
        std::string getText(const std::string &name);
        std::string getTroubleCode(const std::string code);

		template<typename ConstBufferSequence>
        ConstBufferSequence getCommand(const std::string &name)
		{
			if (_getCommandStmt == NULL)
			{
				static char *text = "SELECT Command FROM [Command] WHERE Name=:name AND Catalog=:catalog";
				int ret;
				ret = sqlite3_prepare_v2(_handle, text, strlen(text), &_getCommandStmt, NULL);
				if (ret != SQLITE_OK)
				{
					sqlite3_finalize(_getCommandStmt);
					_getCommandStmt = NULL;
					return ConstBufferSequence();
				}
			}
			if ((sqlite3_reset(_getCommandStmt) == SQLITE_OK) && 
				(sqlite3_bind_text(_getCommandStmt, 1, name.c_str(), name.size(), SQLITE_STATIC) == SQLITE_OK) &&
				(sqlite3_bind_text(_getCommandStmt, 2, _cmdCatalog.c_str(), _cmdCatalog.size(), SQLITE_STATIC) == SQLITE_OK) && 
				(sqlite3_step(_getCommandStmt) == SQLITE_ROW))
			{
				const boost::uint8_t *bytes = (const boost::uint8_t*)sqlite3_column_blob(_getCommandStmt, 0);
				boost::int32_t size = sqlite3_column_bytes(_getCommandStmt, 0);
				return ConstBufferSequence(bytes, size);
			}
			return ConstBufferSequence();
		}

		template<typename ConstBufferSequence>
        ConstBufferSequence getCommandByID(boost::int32_t id)
		{
			if (_getCommandByIDStmt == NULL)
			{
				static char *text = "SELECT Command FROM [Command] WHERE ID=:id";
				int ret = sqlite3_prepare_v2(_handle, text, strlen(text), &_getCommandByIDStmt, NULL);
				if (ret != SQLITE_OK)
				{
					sqlite3_finalize(_getCommandByIDStmt);
					_getCommandByIDStmt = NULL;
					return ConstBufferSequence();
				}
			}

			if ((sqlite3_reset(_getCommandByIDStmt) == SQLITE_OK) &&
				(sqlite3_bind_int(_getCommandByIDStmt, 1, id) == SQLITE_OK) && 
				(sqlite3_step(_getCommandByIDStmt) == SQLITE_ROW))
			{
				const boost::uint8_t *bytes = (const boost::uint8_t*) sqlite3_column_blob(_getCommandByIDStmt, 0);
				boost::int32_t size = sqlite3_column_bytes(_getCommandByIDStmt, 0);
				return ConstBufferSequence(bytes, size);
			}
			return ConstBufferSequence();
		}

        LiveDataVectorPtr getLiveData(bool showed);
    private:
        Database();
        ~Database();
    private:
		std::string _tcCatalog;
		std::string _ldCatalog;
		std::string _cmdCatalog;
        sqlite3 *_handle;
        sqlite3_stmt *_getTextStmt;
        sqlite3_stmt *_getTroubleCodeStmt;
        sqlite3_stmt *_getCommandStmt;
        sqlite3_stmt *_getCommandByIDStmt;
        sqlite3_stmt *_getLiveDataStmt;
    };
}

#endif