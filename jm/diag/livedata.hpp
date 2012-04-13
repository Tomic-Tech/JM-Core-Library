#ifndef __JM_DIAG_LIVE_DATA_HPP__
#define __JM_DIAG_LIVE_DATA_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <jm/types.hpp>

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
    LiveData& operator=(const LiveData &other);
    JMCORE_API std::string shortName() const;
    JMCORE_API void setShortName(const std::string &shortName);
    JMCORE_API std::string content() const;
    JMCORE_API void setContent(const std::string &content);
    JMCORE_API std::string unit() const;
    JMCORE_API void setUnit(const std::string &unit);
    JMCORE_API std::string defaultValue() const;
    JMCORE_API void setDefaultValue(const std::string &defaultValue);
    JMCORE_API std::string value() const;
    JMCORE_API void setValue(const std::string &value);
    JMCORE_API boost::int32_t cmdID() const;
    JMCORE_API void setCmdID(boost::int32_t id);
    JMCORE_API bool enabled() const;
    JMCORE_API void setEnabled(bool enabled);
    JMCORE_API bool showed() const;
    JMCORE_API void setShowed(bool showed);
    JMCORE_API void setIndex(boost::int32_t index);
    JMCORE_API boost::int32_t index();

    boost::signals2::signal<void (std::string)> valueChanged;
    boost::signals2::signal<void (bool)> enabledChanged;
    boost::signals2::signal<void (bool)> showedChanged;
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

#endif