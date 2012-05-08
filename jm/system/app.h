#ifndef __JM_SYSTEM_APP_H__
#define __JM_SYSTEM_APP_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/livedatavector.h>
#include <jm/system/register.h>
#include <jm/vehicle/database.h>
#include <jm/vehicle/loader.h>
#include <jm/ui/message.h>
#include <jm/ui/menu.h>
#include <jm/ui/troublecode.h>
#include <jm/ui/livedataprepare.h>
#include <jm/ui/livedataview.h>
#include <jm/ui/msgbox.h>
#include <jm/ui/statusbox.h>
#include <jm/diag/livedatavector.h>
#include <jm/diag/boxfactory.h>
#include <jm/diag/boxstream.h>
#include <jm/diag/CalcStdObdTroubleCode.h>

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
    JMCORE_API JM::System::Register & reg();
    JMCORE_API JM::Vehicle::Database & vdb();
    JMCORE_API JM::Vehicle::Loader & vldr();
    JMCORE_API JM::UI::Message & ui();
    JMCORE_API JM::Diag::BoxFactory & box();
    JMCORE_API JM::Diag::BoxStream & stream();
    Diag::LiveDataVectorPtr ldVectorPtr;
private:
    App();
    ~App();
private:
    AppPrivate *_priv;
};

inline App & app()
{
    return App::inst();
}

}
}

#endif /* __JM_SYSTEM_APP_H__ */
