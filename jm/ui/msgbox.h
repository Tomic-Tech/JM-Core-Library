#ifndef __JM_UI_MSG_BOX_H__
#define __JM_UI_MSG_BOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace UI
{

class MsgBox : public boost::noncopyable
{
public:
    JMCORE_API MsgBox(const std::string &title, const std::string &msg);
    JMCORE_API ~MsgBox();
    JMCORE_API void add_btns(const StringVector &btns);
    JMCORE_API std::string exec();
private:
    std::string _title;
    std::string _msg;
    StringVector _btns;
};

}
}

#endif /* __JM_UI_MSG_BOX_H__ */
