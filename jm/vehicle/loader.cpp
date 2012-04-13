#include "loader.hpp"
#include <sstream>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include "jm/system/register.hpp"
#include "jm/system/database.hpp"
#include "jm/ui/message.hpp"
#include "jm/diag/boxfactory.hpp"
#include "database.hpp"
#include "base.hpp"

#ifdef BOOST_WINDOWS
#include <Windows.h>
#endif

namespace JM
{
namespace Vehicle
{

class LoaderPrivate
{
    friend class Loader;
private:
    LoaderPrivate()
        : _path()
        , _openCommboxSuccess(false)
        , _hnd(NULL)
    {

    }

    bool load(const std::string &fileName, const std::string &path)
    {
        if (_hnd)
            return true;
        if (fileName.empty())
            return false;

        std::stringstream filePath;
        filePath << _path << "/" << path << "/" << fileName;

        return loadSys(filePath.str());
    }

    bool unload()
    {
        if (_hnd)
        {
            return false;
        }

        if (unloadSys())
        {
            _hnd = NULL;
        }

        return _hnd == NULL;
    }

    void* resolve(const std::string &symbol)
    {
        if (!_hnd)
            return NULL;

        return resolveSys(symbol);
    }

    bool openDB(const std::string &path, const std::string &name)
    {
        std::stringstream absDBPath;
        absDBPath << _path << "/" << path << "/" << name << ".db";

        std::string pw = System::Register::inst().decrypt(System::Register::DB_PW);

        return Database::inst().open(absDBPath.str(), pw);
    }

    bool openCommbox()
    {
        std::string text = System::Database::inst().text("OpenCommbox");
        UI::Message::inst().msgBoxBtnClr();
        UI::Message::inst().msgBoxSetMsg(text);
        UI::Message::inst().msgBoxShow();

        Diag::BoxVersionPtr box = Diag::BoxFactory::inst().getBox();
        if (box.get() == NULL)
        {
            UI::Message::inst().msgBoxHide();
            return false;
        }

        boost::system::error_code ec;

        box->open(ec);

        if (ec)
        {
            UI::Message::inst().msgBoxHide();
            return false;
        }

        UI::Message::inst().msgBoxHide();
        _openCommboxSuccess = true;
        return true;
    }

    bool closeCommbox()
    {
        if (!_openCommboxSuccess)
            return true;

        std::string text = System::Database::inst().text("CloseCommbox");
        UI::Message::inst().msgBoxBtnClr();
        UI::Message::inst().msgBoxSetMsg(text);
        UI::Message::inst().msgBoxShow();

        Diag::BoxVersionPtr box = Diag::BoxFactory::inst().getBox();
        if (box.get() == NULL)
        {
            UI::Message::inst().msgBoxHide();
            return false;
        }

        boost::system::error_code ec;

        box->close(ec);

        if (ec)
        {
            UI::Message::inst().msgBoxHide();
            return false;
        }

        UI::Message::inst().msgBoxHide();
        _openCommboxSuccess = false;
        return true;
    }

#ifdef BOOST_WINDOWS

    bool loadSys(const std::string &fileName)
    {
        std::string attempt = fileName + ".dll";
        // avoid 'Bad Image' message box
        UINT oldmode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
        _hnd = LoadLibraryA(attempt.c_str());

        if (!_hnd && ::GetLastError() == ERROR_MOD_NOT_FOUND)
        {
            std::string secondAttempt = fileName;
            _hnd = LoadLibraryA(secondAttempt.c_str());
        }

        SetErrorMode(oldmode);

        if (!_hnd)
        {
            // Cannot load library
            return false;
        }

        char buffer[MAX_PATH];
        ::GetModuleFileNameA(_hnd, buffer, MAX_PATH);
        attempt = std::string(buffer);

        return true;
    }

    bool unloadSys()
    {
        if (!FreeLibrary(_hnd))
        {
            return false;
        }
        return true;
    }

    void* resolveSys(const std::string &symbol)
    {
        void* address = (void*)GetProcAddress(_hnd, symbol.c_str());
        return address;
    }

#endif
private:
    std::string _path;
    bool _openCommboxSuccess;

#ifdef BOOST_WINDOWS
    HINSTANCE _hnd;
#endif
};

Loader::Loader()
    : _priv(new LoaderPrivate())
{

}

Loader::~Loader()
{
    delete _priv;
}

Loader& Loader::inst()
{
    static Loader instance;
    return instance;
}

bool Loader::run(const std::string &name,
                 const std::string &path,
                 const std::string &dbName)
{
    if (!_priv->openDB(path, dbName))
        return false;

    _priv->_openCommboxSuccess = _priv->openCommbox();

    if (!_priv->_openCommboxSuccess)
    {
        std::string text = System::Database::inst().text("No Commbox, Continue");
        std::string go = System::Database::inst().text("Go");
        std::string back = System::Database::inst().text("Back");

        UI::Message::inst().msgBoxBtnClr();
        UI::Message::inst().msgBoxAddBtn(go);
        UI::Message::inst().msgBoxAddBtn(back);
        UI::Message::inst().msgBoxSetMsg(text);
        UI::Message::inst().msgBoxShow();

        std::string input = UI::Message::inst().btnClicked(true);
        UI::Message::inst().msgBoxHide();
        if (input.compare(back) == 0)
            return false;

    }

    if (_priv->load(name, path))
    {
        MainFunc func = (MainFunc)_priv->resolve("DiagMain");
        if (func != NULL)
            func();
    }

    _priv->closeCommbox();
    return true;
}

void Loader::setPath(const std::string &appPath)
{
    std::vector<std::string> strArray;

    strArray = boost::split(strArray, appPath, boost::is_any_of("\\/"));

    for (std::vector<std::string>::iterator it = strArray.begin(); it != strArray.end(); ++it)
    {
        if (it != strArray.begin())
        {
            _priv->_path += "/";
        }
        _priv->_path += *it;
    }

    _priv->_path += "vehicles";
}

std::string Loader::path()
{
    return _priv->_path;
}

}
}