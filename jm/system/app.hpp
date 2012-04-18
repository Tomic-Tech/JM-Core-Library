#ifndef __JM_SYSTEM_APP_HPP__
#define __JM_SYSTEM_APP_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <jm/types.hpp>
#include <jm/system/register.hpp>
#include <jm/vehicle/database.hpp>
#include <jm/vehicle/loader.hpp>
#include <jm/ui/message.hpp>
#include <jm/ui/menu.hpp>
#include <jm/ui/troublecode.hpp>
#include <jm/ui/livedataprepare.hpp>
#include <jm/ui/livedataview.hpp>
#include <jm/ui/msgbox.hpp>
#include <jm/ui/statusbox.hpp>
#include <jm/diag/livedatavector.hpp>
#include <jm/diag/boxfactory.hpp>
#include <jm/diag/commbox.hpp>
#include <jm/diag/calcstdobdtroublecode.hpp>

namespace JM
{
namespace System
{

class AppPrivate;
class App
{
public:
    static JMCORE_API App& inst();
    JMCORE_API void init(const std::string &appPath);
    JMCORE_API JM::System::Register& reg();
    JMCORE_API JM::Vehicle::Database& vdb();
    JMCORE_API JM::Vehicle::Loader& vldr();
    JMCORE_API JM::UI::Message& ui();
    JMCORE_API JM::Diag::BoxFactory& box();
    JMCORE_API JM::Diag::Commbox& commbox();
    Diag::LiveDataVectorPtr ldVecPtr;
private:
    App();
    ~App();
private:
    AppPrivate *_priv;
};

inline App& app()
{
    return App::inst();
}

}
}

#endif