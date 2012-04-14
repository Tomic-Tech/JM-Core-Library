#ifndef __JM_UI_LIVE_DATA_HPP__
#define __JM_UI_LIVE_DATA_HPP__

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
class LiveDataView
{
public:
    typedef boost::function<bool (const std::string &)> Func;
public:
    JMCORE_API LiveDataView();
    JMCORE_API ~LiveDataView();
    void JMCORE_API addBtns(const StringVector &btns);
    void JMCORE_API setPtr(const Diag::LiveDataVectorPtr &ptr);
    void JMCORE_API operator()();
    Func btnClickedEvent;
private:
    Diag::LiveDataVectorPtr _ptr;
    StringVector _btns;
};
}
}

#endif