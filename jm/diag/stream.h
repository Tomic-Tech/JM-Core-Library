#ifndef __JM_DIAG_STREAM_H__
#define __JM_DIAG_STREAM_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace Diag
{

class Stream : public boost::noncopyable
{
public:
    std::size_t bytes_available();

    template<typename MutableBufferSequence>
    std::size_t read(const MutableBufferSequence &data, boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);

        if (_queue.empty())
        {
            ec = boost::asio::error::no_data;
            return 0;
        }

        boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);
        std::size_t dataCount = boost::asio::buffer_size(data);
        std::size_t count = dataCount > _queue.size() ? _queue.size() : dataCount;

        for (std::size_t i = 0; i < count; ++i)
        {
            p[i] = _queue.front();
            _queue.pop_front();
        }
        return count;
    }

    template<typename ConstBufferSequence>
    std::size_t write(const ConstBufferSequence &data,
                      boost::system::error_code &ec)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);

        std::size_t count = boost::asio::buffer_size(data);

        if (count <= 0)
        {
            ec = boost::asio::error::no_data;
            return 0;
        }

        const boost::uint8_t *p = boost::asio::buffer_cast<const boost::uint8_t *>(data);

        for (std::size_t i = 0; i < count; ++i)
        {
            _queue.push_back(p[i]);
        }

        return count;
    }

    void clear();
private:
    BytesQueue _queue;
    boost::mutex _mutex;
};

}
}


#endif