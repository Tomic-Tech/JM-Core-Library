#ifndef __JM_LD_VECTOR_HPP__
#define __JM_LD_VECTOR_HPP__

#include <vector>
#include <hash_map>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include "jmlivedata.hpp"

namespace JM
{
	class LiveDataVector
	{
	public:
		static void updateGlobalArray(bool showed);
		static boost::int32_t nextShowedIndex();
		static boost::int32_t getEnabledIndex(boost::int32_t index);
		static boost::int32_t queryShowedPosition(boost::int32_t index);
		static boost::int32_t getShowedIndex(boost::int32_t index);
		static void deployEnabledIndex();
		static void deployShowedIndex();
		static std::size_t globalSize();
		static std::size_t enabledSize();
		static std::size_t showedSize();
		static LiveDataPtr globalAt(boost::int32_t index);
		static void setShowed(boost::int32_t index, bool showed);
		static void setEnabled(boost::int32_t index, bool enabled);
	public:
		LiveDataVector();
		~LiveDataVector();
		void push_back(const LiveDataPtr &ptr);

	private:
		std::vector<LiveDataPtr> _vector;
		std::vector<boost::int32_t> _showIndexes;
		std::hash_map<boost::int32_t, boost::int32_t> _showPositions;
		std::vector<boost::int32_t> _enabledIndexes;
		boost::int32_t _currentEnabledIndex;
		std::size_t _enabledSize;
		std::size_t _showSize;
		static boost::mutex _mutex;
		static boost::shared_ptr<LiveDataVector> _ldVector;
	};

	typedef boost::shared_ptr<LiveDataVector> LiveDataVectorPtr;
}

#endif