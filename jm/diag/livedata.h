#ifndef __JM_DIAG_LIVE_DATA_H__
#define __JM_DIAG_LIVE_DATA_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace Diag
{

class LiveData
{
public:
    LiveData();
    LiveData(const std::string &shortName,
              const std::string &content,
              const std::string &unit,
              const std::string &defaultValue,
              boost::int32_t cmdID,
              bool showed);
    LiveData(const LiveData &other);
    LiveData & operator=(const LiveData &other);
    JMCORE_API std::string short_name() const;
    JMCORE_API void set_short_name(const std::string &shortName);
    JMCORE_API std::string content() const;
    JMCORE_API void set_content(const std::string &content);
    JMCORE_API std::string unit() const;
    JMCORE_API void set_unit(const std::string &unit);
    JMCORE_API std::string default_value() const;
    JMCORE_API void set_default_value(const std::string &defaultValue);
    JMCORE_API std::string value() const;
    JMCORE_API void set_value(const std::string &value);
    JMCORE_API boost::int32_t cmd_id() const;
    JMCORE_API void set_cmd_id(boost::int32_t id);
    JMCORE_API bool enabled() const;
    JMCORE_API void set_enabled(bool enabled);
    JMCORE_API bool showed() const;
    JMCORE_API void set_showed(bool showed);
    JMCORE_API void set_index(boost::int32_t index);
    JMCORE_API boost::int32_t index();

    boost::signals2::signal<void (std::string) > value_changed;
    boost::signals2::signal<void (bool) > enabled_changed;
    boost::signals2::signal<void (bool) > showed_changed;
private:
    std::string _shortName;
    std::string _content;
    std::string _unit;
    std::string _defaultValue;
    std::string _value;
    boost::int32_t _cmdID;
    bool _enabled; // If the value is FALSE, user can't choose this live data.
    bool _showed; // User can set this TRUE or FALSE to determine this live data show or not.
    boost::int32_t _index; // Index of the live data vector
};

typedef boost::shared_ptr<LiveData> LiveDataPtr;

}
}

#endif /* __JM_DIAG_LIVE_DATA_H__ */
