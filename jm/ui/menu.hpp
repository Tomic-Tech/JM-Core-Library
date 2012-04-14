#ifndef __JM_UI_MENU_HPP__
#define __JM_UI_MENU_HPP__

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <jm/types.hpp>

namespace JM
{
namespace UI
{
class Menu
{
public:
    typedef boost::function<bool (const std::string &input)> Func;
public:
    explicit JMCORE_API Menu();
    JMCORE_API ~Menu();
    void JMCORE_API addItems(const StringVector &strVec);
    std::string JMCORE_API selected();
    void JMCORE_API operator()();
    Func selectEvent;
private:
    StringVector _strVec;
};
}
}

#endif