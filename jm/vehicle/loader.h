#ifndef __JM_VEHICLE_LOADER_H__
#define __JM_VEHICLE_LOADER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace Vehicle
{
class LoaderPrivate;
class Loader : public boost::noncopyable
{
public:
    static JMCORE_API Loader& inst();
    JMCORE_API bool run(const std::string &name,
                        const std::string &path,
                        const std::string &dbName);
    void set_path(const std::string &appPath);
    JMCORE_API std::string path();
private:
    Loader();
    ~Loader();
private:
    LoaderPrivate *_priv;
};
}
}

#endif /* __JM_VEHICLE_LOADER_H__ */
