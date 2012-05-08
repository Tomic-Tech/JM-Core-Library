#ifndef __JM_UI_LIVE_DATA_VIEW_H__
#define __JM_UI_LIVE_DATA_VIEW_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/livedatavector.h>

namespace JM
{
namespace UI
{

class LiveDataView : public boost::noncopyable
{
public:
    typedef boost::function<bool (const std::string &)> Func;
public:
    JMCORE_API LiveDataView();
    JMCORE_API ~LiveDataView();
    void JMCORE_API add_btns(const StringVector &btns);
    void JMCORE_API set_ptr(const Diag::LiveDataVectorPtr &ptr);
    void JMCORE_API operator() ();
    Func btnClickedEvent;
private:
    Diag::LiveDataVectorPtr _ptr;
    StringVector _btns;
};

}
}

#endif /* __JM_UI_LIVE_DATA_VIEW_H__ */
