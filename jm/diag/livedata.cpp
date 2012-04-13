#include "livedata.hpp"


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

LiveData& LiveData::operator = (const LiveData &other)
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

std::string LiveData::shortName() const
{
    return _shortName;
}

void LiveData::setShortName(const std::string &shortName)
{
    _shortName = shortName;
}

std::string LiveData::content() const
{
    return _content;
}

void LiveData::setContent(const std::string &content)
{
    _content = content;
}

std::string LiveData::unit() const
{
    return _unit;
}

void LiveData::setUnit(const std::string &unit)
{
    _unit = unit;
}

std::string LiveData::defaultValue() const
{
    return _defaultValue;
}

void LiveData::setDefaultValue(const std::string &defaultValue)
{
    _defaultValue = defaultValue;
}

std::string LiveData::value() const
{
    return _value;
}

void LiveData::setValue(const std::string &value)
{
    _value = value;
    valueChanged(_value);
}

boost::int32_t LiveData::cmdID() const
{
    return _cmdID;
}

void LiveData::setCmdID(boost::int32_t id)
{
    _cmdID = id;
}

bool LiveData::enabled() const
{
    return _enabled;
}

void LiveData::setEnabled(bool enabled)
{
    _enabled = enabled;
    enabledChanged(_enabled);
}

bool LiveData::showed() const
{
    return _showed;
}

void LiveData::setShowed(bool showed)
{
    _showed = showed;
    showedChanged(_showed);
}

void LiveData::setIndex(boost::int32_t index)
{
    _index = index;
}

boost::int32_t LiveData::index()
{
    return _index;
}
}
}