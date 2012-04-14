#ifndef __JM_UI_TROUBLE_CODE_HPP__
#define __JM_UI_TROUBLE_CODE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <jm/types.hpp>

namespace JM
{
namespace UI
{
typedef std::pair<std::string, std::string> TC;
typedef std::vector<TC> TCVector;
class TroubleCode
{
public:
    typedef boost::function<bool (const std::string &btn) > Func;
    typedef boost::function<void () > FirstFunc;
public:
    JMCORE_API TroubleCode();
    JMCORE_API ~TroubleCode();
    JMCORE_API void addItems(const TCVector &vec);
    JMCORE_API void addBtns(const StringVector &btns);
    JMCORE_API std::string btnClicked();
    Func btnClickedEvent;
    FirstFunc firstFuncEvent;
    JMCORE_API void operator()();
private:
    StringVector _btns;
};
}
}

#endif