#ifndef __JM_VEHICLE_LOADER_HPP__
#define __JM_VEHICLE_LOADER_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <boost/noncopyable.hpp>
#include <jm/types.hpp>

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
    void setPath(const std::string &appPath);
    JMCORE_API std::string path();
private:
    Loader();
    ~Loader();
private:
    LoaderPrivate *_priv;
};
}
}

#endif