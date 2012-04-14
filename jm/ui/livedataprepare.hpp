#ifndef __JM_UI_LIVE_DATA_PREPARE_HPP__
#define __JM_UI_LIVE_DATA_PREPARE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <jm/types.hpp>
#include <jm/diag/livedatavector.hpp>

namespace JM
{
namespace UI
{
class LiveDataPrepare
{
public:
    typedef boost::function<void ()> Func;
public:
    explicit JMCORE_API LiveDataPrepare();
    JMCORE_API ~LiveDataPrepare();
    JMCORE_API void setPtr(const Diag::LiveDataVectorPtr &ptr);
    JMCORE_API bool enter();
    JMCORE_API void operator()();
    Func enterEvent;
private:
    Diag::LiveDataVectorPtr _ptr;
};
}
}

#endif