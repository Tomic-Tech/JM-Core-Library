#include "loader.h"
#include <sstream>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include "jm/system/register.h"
#include "jm/system/database.h"
#include "jm/ui/message.h"
#include "jm/ui/statusbox.h"
#include "jm/ui/msgbox.h"
#include "jm/diag/boxfactory.h"
#include "database.h"
#include "base.h"

#ifdef BOOST_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
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
        if (!_hnd)
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
        UI::StatusBox statusBox("", text);
        statusBox.show();

        Diag::CommboxPtr box = Diag::BoxFactory::inst().get_box();
        if (box.get() == NULL)
        {
            statusBox.hide();
            return false;
        }

        boost::system::error_code ec;

        box->open(ec);

        if (ec)
        {
            return false;
        }

        _openCommboxSuccess = true;
        return true;
    }

    bool closeCommbox()
    {
        if (!_openCommboxSuccess)
            return true;

        std::string text = System::Database::inst().text("CloseCommbox");
        UI::StatusBox statusBox("", text);
        statusBox.show();

        Diag::CommboxPtr box = Diag::BoxFactory::inst().get_box();
        if (box.get() == NULL)
        {
            return false;
        }

        boost::system::error_code ec;

        box->close(ec);

        if (ec)
        {
            return false;
        }

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
#else
    bool loadSys(const std::string &fileName)
    {
        std::string attempt = fileName + ".so";
        int dlFlags = RTLD_NOW;
        _hnd = (void*)dlopen(attempt.c_str(), dlFlags);

        if (!_hnd)
        {
            // Cannot load library
            return false;
        }

        return true;
    }

    bool unloadSys()
    {
        if (dlclose(_hnd))
        {
            return false;
        }
        return true;
    }

    void* resolveSys(const std::string &symbol)
    {
        void* address = (void*)dlsym(_hnd, symbol.c_str());
        return address;
    }
#endif
private:
    std::string _path;
    bool _openCommboxSuccess;

#ifdef BOOST_WINDOWS
    HINSTANCE _hnd;
#else
    void * _hnd;
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
        StringVector btns;
        btns.push_back(go);
        btns.push_back(back);

        UI::MsgBox msgBox("", text);
        msgBox.add_btns(btns);
        std::string ret = msgBox.exec();

        if (ret.compare(back) == 0)
            return false;

    }

    if (_priv->load(name, path))
    {
        MainFunc func = (MainFunc)_priv->resolve("DiagMain");
        if (func != NULL)
            func();
    }

    _priv->closeCommbox();
    _priv->unload();
    return true;
}

void Loader::set_path(const std::string &appPath)
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
