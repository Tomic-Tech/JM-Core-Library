#include "app.hpp"
#include "jm/system/database.hpp"
#include "jm/system/register.hpp"

namespace JM
{
namespace System
{
class AppPrivate
{
    friend class App;
private:

};
App::App()
{

}

App::~App()
{

}

App& App::inst()
{
    static App instance;
    return instance;
}

void App::init(const std::string &appPath)
{
    Database::inst().setPath(appPath);
    reg().setPath(appPath);
    vldr().setPath(appPath);
}

JM::System::Register& App::reg()
{
    return Register::inst();
}

JM::Vehicle::Database& App::vdb()
{
    return Vehicle::Database::inst();
}

JM::Vehicle::Loader& App::vldr()
{
    return Vehicle::Loader::inst();
}

JM::UI::Message& App::ui()
{
    return JM::UI::Message::inst();
}

JM::Diag::BoxFactory& App::box()
{
    return JM::Diag::BoxFactory::inst();
}

JM::Diag::Commbox& App::commbox()
{
    return JM::Diag::Commbox::inst();
}

}
}