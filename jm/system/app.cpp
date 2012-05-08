#include "App.h"
#include "database.h"

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
    JM::System::Database::inst().set_path(appPath);
    reg().set_path(appPath);
    vldr().set_path(appPath);
}

JM::System::Register& App::reg()
{
    return JM::System::Register::inst();
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

JM::Diag::BoxStream& App::stream()
{
    return JM::Diag::BoxStream::inst();
}

}
}
