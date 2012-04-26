#ifndef __JM_VEHICLE_BASE_HPP__
#define __JM_VEHICLE_BASE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>
#include <string>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace JM
{
namespace Vehicle
{
typedef void (*MainFunc)();
typedef boost::function<void ()> VoidFunc0;
typedef std::map<std::string, VoidFunc0> VoidFunc0Map;
class Base : public boost::noncopyable
{
public:
    virtual void run() = 0;
};
}
}

#endif