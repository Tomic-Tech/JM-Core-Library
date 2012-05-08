#ifndef __JM_DIAG_LIVE_DATA_VECTOR_H__
#define __JM_DIAG_LIVE_DATA_VECTOR_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/livedata.h>

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
    JMCORE_API boost::int32_t next_showed_index();
    JMCORE_API boost::int32_t get_enabled_index(boost::int32_t index);
    JMCORE_API boost::int32_t query_showed_position(boost::int32_t index);
    JMCORE_API boost::int32_t get_showed_index(boost::int32_t index);
    JMCORE_API void deploy_enabled_index();
    JMCORE_API void deploy_showed_index();
    JMCORE_API std::size_t size();
    JMCORE_API std::size_t enabled_size();
    JMCORE_API std::size_t showed_size();
    JMCORE_API const LiveDataPtr& at(std::size_t index);
    JMCORE_API const LiveDataPtr& operator[] (std::size_t index);
private:
    void set_value(boost::int32_t index, const std::string &value);
    void set_showed(boost::int32_t index, bool showed);
    void set_enabled(boost::int32_t index, bool enabled);
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

#endif /* __JM_DIAG_LIVE_DATA_VECTOR_H__ */
