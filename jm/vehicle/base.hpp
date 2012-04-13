#ifndef __JM_VEHICLE_BASE_HPP__
#define __JM_VEHICLE_BASE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/noncopyable.hpp>

namespace JM
{
namespace Vehicle
{

typedef void (*MainFunc)(void);

class Base : public boost::noncopyable
{
public:
    virtual void run() = 0;
};
}
}

#endif