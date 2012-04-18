#ifndef __JM_UI_MSG_BOX_HPP__
#define __JM_UI_MSG_BOX_HPP__

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
class MsgBox
{
public:
    JMCORE_API MsgBox(const std::string &title, const std::string &msg);
    JMCORE_API ~MsgBox();
    JMCORE_API void addBtns(const StringVector &btns);
    JMCORE_API std::string exec();
private:
    std::string _title;
    std::string _msg;
    StringVector _btns;
};
}
}

#endif