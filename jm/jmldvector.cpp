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

	void LiveDataVector::updateGlobalArray(bool showed)
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
			_ldVector.reset();

		_ldVector = JM::Database::inst().getLiveData(showed);
	}

	boost::int32_t LiveDataVector::nextShowedIndex_()
	{
		if (_showIndexes.size() == 0)
			return -1;

		std::size_t size = _showIndexes.size();
		boost::int32_t ret = _showIndexes[_currentEnabledIndex];
		_currentEnabledIndex++;

		if ((std::size_t)_currentEnabledIndex > (size - 1))
			_currentEnabledIndex = 0;

		return ret;
	}

	boost::int32_t LiveDataVector::nextShowedIndex()
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			return _ldVector->nextShowedIndex_();
		}
		return -1;
	}

	boost::int32_t LiveDataVector::getEnabledIndex_(boost::int32_t index)
	{
		if (index > _enabledIndexes.size())
			return -1;

		return _enabledIndexes[index];
	}

	boost::int32_t LiveDataVector::getEnabledIndex(boost::int32_t index)
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			return _ldVector->getEnabledIndex_(index);
		}
		return -1;
	}

	boost::int32_t LiveDataVector::queryShowedPosition_(boost::int32_t index)
	{
		return _showPositions[index];
	}

	boost::int32_t LiveDataVector::queryShowedPosition(boost::int32_t index)
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			return _ldVector->queryShowedPosition_(index);
		}
		return -1;
	}

	boost::int32_t LiveDataVector::getShowedIndex_(boost::int32_t index)
	{
		if (index > _showIndexes.size())
			return -1;

		return _showIndexes[index];
	}

	boost::int32_t LiveDataVector::getShowedIndex(boost::int32_t index)
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			return _ldVector->getShowedIndex_(index);
		}
		return -1;
	}

	void LiveDataVector::deployEnabledIndex_()
	{
		_enabledIndexes.clear();

		for (std::size_t i = 0; i < size(); i++)
		{
			LiveDataPtr ld = at(i);
			if (ld->enabled())
				_enabledIndexes.push_back(i);
		}
	}

	void LiveDataVector::deployEnabledIndex()
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			_ldVector->deployEnabledIndex_();
		}
	}

	void LiveDataVector::deployShowedIndex_()
	{
		_showIndexes.clear();

		std::size_t j = 0;
		for (std::size_t i = 0; i < size(); i++)
		{
			LiveDataPtr ld = at(i);

			if (ld->enabled() && ld->showed())
			{
				_showIndexes.push_back(i);
				_showPositions[i, j++];
			}
		}

		_currentEnabledIndex = _showIndexes[0];
	}

	void LiveDataVector::deployShowedIndex()
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
		{
			_ldVector->deployShowedIndex_();
		}
	}

	size_t LiveDataVector::globalSize()
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->size();

		return 0;
	}

	size_t LiveDataVector::enabledSize_()
	{
		return _enabledIndexes.size();
	}

	size_t LiveDataVector::enabledSize()
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->enabledSize_();

		return 0;
	}

	size_t LiveDataVector::showedSize_()
	{
		return _showIndexes.size();
	}

	size_t LiveDataVector::showedSize()
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->showedSize_();

		return 0;
	}

	LiveDataPtr LiveDataVector::globalAt(boost::int32_t index)
	{
		boost::mutex::scoped_lock(_mutex);
		if (_ldVector.get() != NULL)
			return _ldVector->at(index);

		return boost::make_shared<LiveData>();
	}
}