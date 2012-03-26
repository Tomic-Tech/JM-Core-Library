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
        boost::asio::const_buffer getCommand(const std::string &name);
        boost::asio::const_buffer getCommandByID(boost::int32_t id);
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