#ifndef __JM_SYS_HPP__
#define __JM_SYS_HPP__

#include <sqlite/sqlite3.h>
#include <string>
#include "jmauth.hpp"

namespace JM
{
    class System
    {
    public:
        static System& inst();
		void setPath(const std::string &path);
		std::string text(const std::string &name);
    private:
        System();
        ~System();
    private:
        std::string _path;
        sqlite3 *_db;
        sqlite3_stmt *_getTextStmt;
    };
}

#endif

