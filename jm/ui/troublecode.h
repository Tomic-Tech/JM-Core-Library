#ifndef __JM_UI_TROUBLE_CODE_H__
#define __JM_UI_TROUBLE_CODE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace UI
{

typedef std::pair<std::string, std::string> TC;
typedef std::vector<TC> TCVector;

class TroubleCode
{
public:
    typedef boost::function<bool (const std::string &) > Func;
    typedef boost::function<void () > FirstFunc;
public:
    JMCORE_API TroubleCode();
    JMCORE_API ~TroubleCode();
    JMCORE_API TroubleCode(const TroubleCode &tc);
    JMCORE_API TroubleCode & operator=(const TroubleCode &ec);
    JMCORE_API void add_items(const TCVector &vec);
    JMCORE_API void add_btns(const StringVector &btns);
    JMCORE_API std::string btn_clicked();
    Func btnClickedEvent;
    FirstFunc firstFuncEvent;
    JMCORE_API void operator() ();
private:
    StringVector _btns;
};

}
}

#endif /* __JM_UI_TROUBLE_CODE_H__ */
