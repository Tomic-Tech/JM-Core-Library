#include "jmldvector.hpp"
#include "jmdb.hpp"

namespace JM
{
	boost::mutex LiveDataVector::_mutex;
	boost::shared_ptr<LiveDataVector> LiveDataVector::_ldVector = boost::make_shared<LiveDataVector>();

	LiveDataVector::LiveDataVector()
		: _showIndexes()
		, _showPositions()
		, _enabledIndexes()
		, _currentEnabledIndex(-1)
		, _enabledSize(0)
		, _showSize(0)
	{

	}

	LiveDataVector::~LiveDataVector()
	{

	}

	void LiveDataVector::push_back(const LiveDataPtr &ptr)
	{
		_vector.push_back(ptr);
		if (ptr->enabled())
		{
			++_enabledSize;
			if (ptr->showed())
			{
				++_showSize;
			}
		}
	}

	void LiveDataVector::updateGlobalArray(bool showed)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		_ldVector = (JM::Database::inst().getLiveData(showed));
	}

	boost::int32_t LiveDataVector::nextShowedIndex()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			if (_ldVector->_showIndexes.size() == 0)
				return -1;

			std::size_t size = _ldVector->_showIndexes.size();
			boost::int32_t ret = _ldVector->_showIndexes[_ldVector->_currentEnabledIndex];
			++(_ldVector->_currentEnabledIndex);

			if ((std::size_t)_ldVector->_currentEnabledIndex > (size - 1))
				_ldVector->_currentEnabledIndex = 0;
			return ret;
		}
		return -1;
	}

	boost::int32_t LiveDataVector::getEnabledIndex(boost::int32_t index)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			if ((std::size_t)index > _ldVector->_enabledIndexes.size())
				return -1;

			return _ldVector->_enabledIndexes[index];
		}
		return -1;
	}

	boost::int32_t LiveDataVector::queryShowedPosition(boost::int32_t index)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			return _ldVector->_showPositions[index];
		}
		return -1;
	}

	boost::int32_t LiveDataVector::getShowedIndex(boost::int32_t index)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			if ((std::size_t)index > _ldVector->_showIndexes.size())
				return -1;

			return _ldVector->_showIndexes[index];
		}
		return -1;
	}

	void LiveDataVector::deployEnabledIndex()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			_ldVector->_enabledIndexes.clear();

			for (std::size_t i = 0; i < _ldVector->_vector.size(); ++i)
			{
				LiveDataPtr ld = _ldVector->_vector[i];
				if (ld->enabled())
					_ldVector->_enabledIndexes.push_back(i);
			}
		}
	}

	void LiveDataVector::deployShowedIndex()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			_ldVector->_showIndexes.clear();

			std::size_t j = 0;
			for (std::size_t i = 0; i < _ldVector->_vector.size(); ++i)
			{
				LiveDataPtr ld = _ldVector->_vector[i];

				if (ld->enabled() && ld->showed())
				{
					_ldVector->_showIndexes.push_back(i);
					_ldVector->_showPositions[i] = j++;
				}
			}

			_ldVector->_currentEnabledIndex = _ldVector->_showIndexes[0];
		}
	}

	std::size_t LiveDataVector::globalSize()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->_vector.size();

		return 0;
	}

	std::size_t LiveDataVector::enabledSize()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->_enabledIndexes.size();

		return 0;
	}

	std::size_t LiveDataVector::showedSize()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->_showIndexes.size();

		return 0;
	}

	LiveDataPtr LiveDataVector::globalAt(boost::int32_t index)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->_vector[index];

		return boost::make_shared<LiveData>();
	}

	void LiveDataVector::setShowed(boost::int32_t index, bool showed)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			LiveDataPtr ld = _ldVector->_vector[index];

			if (!ld->showed())
			{
				if (ld->enabled() && showed)
				{
					++(_ldVector->_showSize);
				}
			}
			else
			{
				if (ld->enabled() && !showed)
				{
					--_ldVector->_showSize;
				}
			}

			ld->setShowed(showed);
		}
	}

	void LiveDataVector::setEnabled(boost::int32_t index, bool enabled)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);

		if (_ldVector.get() != NULL)
		{
			LiveDataPtr ld = _ldVector->_vector[index];

			if (!ld->enabled())
			{
				if (enabled)
				{
					++(_ldVector->_enabledSize);
					if (ld->showed())
					{
						++(_ldVector->_showSize);
					}
				}
			}
			else
			{
				if (!enabled)
				{
					--_ldVector->_enabledSize;
					if (ld->showed())
					{
						--_ldVector->_showSize;
					}
				}
			}

			ld->setEnabled(enabled);
		}
	}
}