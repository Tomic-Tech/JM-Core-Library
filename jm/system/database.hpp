#ifndef __JM_SYSTEM_DATABASE_HPP__
#define __JM_SYSTEM_DATABASE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <sqlite/sqlite3.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <jm/system/register.hpp>

namespace JM
{
namespace System
{

class Database : public boost::noncopyable
{
public:
    static Database& inst();
    void setPath(const std::string &path);
    std::string text(const std::string &name);
private:
    Database();
    ~Database();
private:
    std::string _path;
    sqlite3 *_db;
    sqlite3_stmt *_getTextStmt;
};
}
}

#endif