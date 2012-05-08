#ifndef __JM_UI_LIVE_DATA_PREPARE_H__
#define __JM_UI_LIVE_DATA_PREPARE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/livedatavector.h>

namespace JM
{
namespace UI
{

class LiveDataPrepare : public boost::noncopyable
{
public:
    typedef boost::function<void ()> Func;
public:
    explicit JMCORE_API LiveDataPrepare();
    JMCORE_API ~LiveDataPrepare();
    JMCORE_API void set_ptr(const Diag::LiveDataVectorPtr &ptr);
    JMCORE_API bool enter();
    JMCORE_API void operator()();
    Func enterEvent;
private:
    Diag::LiveDataVectorPtr _ptr;
};

}
}

#endif /* __JM_UI_LIVE_DATA_PREPARE_H__ */