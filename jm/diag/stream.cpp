#include "stream.h"

namespace JM
{
namespace Diag
{

std::size_t Stream::bytes_available()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    return _queue.size();
}

void Stream::clear()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    _queue.clear();
}

}
}
