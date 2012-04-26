#ifndef __JM_DIAG_LIVE_DATA_VECTOR_HPP__
#define __JM_DIAG_LIVE_DATA_VECTOR_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <jm/diag/livedata.hpp>

namespace JM
{
namespace Diag
{
class LiveDataVector : public boost::noncopyable
{
public:
    LiveDataVector();
    ~LiveDataVector();

    void push_back(const LiveDataPtr &ptr);
    JMCORE_API boost::int32_t nextShowedIndex();
    JMCORE_API boost::int32_t getEnabledIndex(boost::int32_t index);
    JMCORE_API boost::int32_t queryShowedPosition(boost::int32_t index);
    JMCORE_API boost::int32_t getShowedIndex(boost::int32_t index);
    JMCORE_API void deployEnabledIndex();
    JMCORE_API void deployShowedIndex();
    JMCORE_API std::size_t size();
    JMCORE_API std::size_t enabledSize();
    JMCORE_API std::size_t showedSize();
    JMCORE_API const LiveDataPtr& at(std::size_t index);
    JMCORE_API const LiveDataPtr& operator[] (std::size_t index);
private:
    void setValue(boost::int32_t index, const std::string &value);
    void setShowed(boost::int32_t index, bool showed);
    void setEnabled(boost::int32_t index, bool enabled);
private:
    std::vector<LiveDataPtr> _vector;
    std::vector<boost::int32_t> _showIndexes;
    std::map<boost::int32_t, boost::int32_t> _showPositions;
    std::vector<boost::int32_t> _enabledIndexes;
    boost::int32_t _currentEnabledIndex;
    std::size_t _enabledSize;
    std::size_t _showSize;
    boost::mutex _mutex;
};

typedef boost::shared_ptr<LiveDataVector> LiveDataVectorPtr;

}
}

#endif