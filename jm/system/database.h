#ifndef __JM_SYSTEM_DATABASE_H__
#define __JM_SYSTEM_DATABASE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace System
{
class DatabasePrivate;
class Database
{
public:
    static Database & inst();
    void set_path(const std::string &path);
    std::string text(const std::string &name);
private:
    Database();
    ~Database();
private:
    DatabasePrivate *_priv;
};
}
}

#endif
