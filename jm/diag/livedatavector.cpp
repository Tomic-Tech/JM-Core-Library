#include "livedatavector.hpp"
#include "../ui/message.hpp"

namespace JM
{
namespace Diag
{
LiveDataVector::LiveDataVector()
    : _showIndexes()
    , _showPositions()
    , _enabledIndexes()
    , _currentEnabledIndex(-1)
    , _enabledSize(0)
    , _showSize(0)
    , _mutex()
{

}

LiveDataVector::~LiveDataVector()
{

}

void LiveDataVector::setValue(boost::int32_t index, const std::string &value)
{
    UI::Message::inst().ldSetValue(index, value);
}

void LiveDataVector::setShowed(boost::int32_t index, bool showed)
{
    if (_vector[index]->showed())
    {
        if (_vector[index]->enabled() && showed)
        {
            ++_showSize;
        }
    }
    else
    {
        if (_vector[index]->enabled() && !showed)
        {
            --_showSize;
        }
    }
}

void LiveDataVector::setEnabled(boost::int32_t index, bool enabled)
{
    if (!_vector[index]->enabled())
    {
        if (enabled)
        {
            ++_enabledSize;
            if (_vector[index]->showed())
            {
                ++_showSize;
            }
        }
    }
    else
    {
        if (!enabled)
        {
            --_enabledSize;
            if (_vector[index]->showed())
            {
                --_showSize;
            }
        }
    }
}

void LiveDataVector::push_back(const LiveDataPtr &ptr)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    ptr->setIndex(_vector.size());
    _vector.push_back(ptr);

    ptr->valueChanged.connect(boost::bind(&LiveDataVector::setValue, this, _vector.size() - 1, _1));
    ptr->showedChanged.connect(boost::bind(&LiveDataVector::setShowed, this, _vector.size() - 1, _1));
    ptr->enabledChanged.connect(boost::bind(&LiveDataVector::setEnabled, this, _vector.size() - 1, _1));

    if (ptr->enabled())
    {
        ++_enabledSize;
        if (ptr->showed())
        {
            ++_showSize;
        }
    }
}

boost::int32_t LiveDataVector::nextShowedIndex()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    if (_showIndexes.size() == 0)
        return -1;

    std::size_t size = _showIndexes.size();
    boost::int32_t ret = _showIndexes[_currentEnabledIndex];
    ++(_currentEnabledIndex);

    if ((std::size_t)_currentEnabledIndex > (size - 1))
        _currentEnabledIndex = 0;
    return ret;
}

boost::int32_t LiveDataVector::getEnabledIndex(boost::int32_t index)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    if ((std::size_t)index > _enabledIndexes.size())
    {
        return -1;
    }

    return _enabledIndexes[index];
}

boost::int32_t LiveDataVector::queryShowedPosition(boost::int32_t index)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    return _showPositions[index];
}

boost::int32_t LiveDataVector::getShowedIndex(boost::int32_t index)
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    if ((std::size_t)index > _showIndexes.size())
    {
        return -1;
    }

    return _showIndexes[index];
}

void LiveDataVector::deployEnabledIndex()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    _enabledIndexes.clear();
    for (std::size_t i = 0; i < _vector.size(); ++i)
    {
        LiveDataPtr ld = _vector[i];
        if (ld->enabled())
        {
            _enabledIndexes.push_back(i);
        }
    }
}

void LiveDataVector::deployShowedIndex()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    _showIndexes.clear();

    std::size_t j = 0;

    for (std::size_t i = 0; i < _vector.size(); ++i)
    {
        LiveDataPtr ld = _vector[i];

        if (ld->enabled() && ld->showed())
        {
            _showIndexes.push_back(i);
            _showPositions[i] = j++;
        }
    }
    _currentEnabledIndex = _showIndexes[0];
}

std::size_t LiveDataVector::size()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    return _vector.size();
}

std::size_t LiveDataVector::showedSize()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    return _showIndexes.size();
}

const LiveDataPtr& LiveDataVector::at(std::size_t index)
{
    return _vector.at(index);
}

const LiveDataPtr& LiveDataVector::operator [] (std::size_t index)
{
    return _vector[index];
}

std::size_t LiveDataVector::enabledSize()
{
    return _enabledSize;
}

}
}