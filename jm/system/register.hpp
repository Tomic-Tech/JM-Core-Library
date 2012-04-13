#ifndef __JM_SYSTEM_REGISTER_HPP__
#define __JM_SYSTEM_REGISTER_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

#include <boost/cstdint.hpp>
#include <jm/types.hpp>

namespace JM
{
namespace System
{
class RegisterPrivate;
class Register : public boost::noncopyable
{
public:
    enum DecryptType
    {
        ID_CODE = 0,
        COMMBOX_ID,
        REG_TIME,
        EXPIRE_TIME,
        DB_PW,
        LANG,
        LOG_PW,
        MESSAGE_LINES
    };

public:
    static JMCORE_API Register& inst();
    void setPath(const std::string &path);
    JMCORE_API void saveReg(const std::string &reg);
    JMCORE_API std::string queryIDCode();
    JMCORE_API bool checkReg();
    std::string encryptLogText(const std::string &log);
    std::string decrypt(DecryptType index);
private:
    Register();
    ~Register();
private:
    RegisterPrivate *_priv;
};

inline Register& RegisterInst()
{
    return Register::inst();
}

}
}

#endif