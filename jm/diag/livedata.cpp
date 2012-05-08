#include "livedata.h"

namespace JM
{
namespace Diag
{

LiveData::LiveData()
: _shortName()
, _content()
, _unit()
, _defaultValue()
, _value()
, _cmdID(0)
, _enabled(true)
, _showed(true)
, _index(0)
{

}

LiveData::LiveData(const std::string &shortName,
                   const std::string &content,
                   const std::string &unit,
                   const std::string &defaultValue,
                   boost::int32_t cmdID,
                   bool showed)
: _shortName(shortName)
, _content(content)
, _unit(unit)
, _defaultValue(defaultValue)
, _cmdID(cmdID)
, _enabled(true)
, _showed(showed)
{

}

LiveData::LiveData(const LiveData &other)
: _shortName(other._shortName)
, _content(other._content)
, _unit(other._unit)
, _defaultValue(other._defaultValue)
, _value(other._value)
, _cmdID(other._cmdID)
, _enabled(other._enabled)
, _showed(other._showed)
, _index(other._index)
{

}

LiveData& LiveData::operator =(const LiveData &other)
{
    _shortName = other._shortName;
    _content = other._content;
    _unit = other._unit;
    _defaultValue = other._defaultValue;
    _value = other._value;
    _cmdID = other._cmdID;
    _enabled = other._enabled;
    _showed = other._showed;
    _index = other._index;
    return *this;
}

std::string LiveData::short_name() const
{
    return _shortName;
}

void LiveData::set_short_name(const std::string &shortName)
{
    _shortName = shortName;
}

std::string LiveData::content() const
{
    return _content;
}

void LiveData::set_content(const std::string &content)
{
    _content = content;
}

std::string LiveData::unit() const
{
    return _unit;
}

void LiveData::set_unit(const std::string &unit)
{
    _unit = unit;
}

std::string LiveData::default_value() const
{
    return _defaultValue;
}

void LiveData::set_default_value(const std::string &defaultValue)
{
    _defaultValue = defaultValue;
}

std::string LiveData::value() const
{
    return _value;
}

void LiveData::set_value(const std::string &value)
{
    _value = value;
    value_changed(_value);
}

boost::int32_t LiveData::cmd_id() const
{
    return _cmdID;
}

void LiveData::set_cmd_id(boost::int32_t id)
{
    _cmdID = id;
}

bool LiveData::enabled() const
{
    return _enabled;
}

void LiveData::set_enabled(bool enabled)
{
    _enabled = enabled;
    enabled_changed(_enabled);
}

bool LiveData::showed() const
{
    return _showed;
}

void LiveData::set_showed(bool showed)
{
    _showed = showed;
    showed_changed(_showed);
}

void LiveData::set_index(boost::int32_t index)
{
    _index = index;
}

boost::int32_t LiveData::index()
{
    return _index;
}

}
}
