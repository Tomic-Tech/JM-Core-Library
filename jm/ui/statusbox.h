#ifndef __JM_UI_STATUS_BOX_H__
#define __JM_UI_STATUS_BOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace UI
{

class StatusBox : public boost::noncopyable
{
public:
    JMCORE_API StatusBox(const std::string &title, const std::string &msg);
    JMCORE_API ~StatusBox();
    JMCORE_API void show();
    JMCORE_API void hide();
    
private:
    std::string _title;
    std::string _msg;
};

typedef boost::shared_ptr<StatusBox> StatusBoxPtr;

}
}

#endif /* __JM_UI_STATUS_BOX_H__ */
