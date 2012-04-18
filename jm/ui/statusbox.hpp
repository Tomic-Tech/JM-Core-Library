#ifndef __JM_UI_STATUS_BOX_HPP__
#define __JM_UI_STATUS_BOX_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <vector>
#include <jm/types.hpp>

namespace JM
{
namespace UI
{
class StatusBox
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

#endif