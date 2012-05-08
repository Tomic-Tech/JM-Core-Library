#ifndef __JM_UI_MENU_H__
#define __JM_UI_MENU_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace UI
{

class Menu : public boost::noncopyable
{
public:
    typedef boost::function<bool (const std::string &input)> Func;
public:
    explicit JMCORE_API Menu();
    JMCORE_API ~Menu();
    void JMCORE_API add_items(const StringVector &strVec);
    std::string JMCORE_API selected();
    void JMCORE_API operator() ();
    Func selectEvent;
private:
    StringVector _strVec;
};

}
}

#endif /* __JM_UI_MENU_H__ */
