#include "stream.hpp"

namespace JM
{
namespace Diag
{

std::size_t Stream::bytesAvailable()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    return _deque.size();
}

void Stream::clear()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    _deque.clear();
}

}
}