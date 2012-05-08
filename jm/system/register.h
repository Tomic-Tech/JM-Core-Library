#ifndef __JM_SYSTEM_REGISTER_H__
#define __JM_SYSTEM_REGISTER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

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
    void set_path(const std::string &path);
    JMCORE_API void save_reg(const std::string &reg);
    JMCORE_API std::string query_id_code();
    JMCORE_API bool check_reg();
    std::string encrypt_log_text(const std::string &log);
    std::string decrypt(DecryptType index);
private:
    Register();
    ~Register();
private:
    RegisterPrivate *_priv;
};
}
}

#endif /* __JM_SYSTEM_REGISTER_H__ */