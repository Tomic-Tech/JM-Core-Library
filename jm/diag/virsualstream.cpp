#include "virsualstream.hpp"

namespace JM
{
namespace Diag
{

VisualStream::VisualStream(Stream &toCommbox,
                           Stream &fromCommbox,
                           boost::mutex &mutex,
                           boost::condition_variable &cond)
    : _toCommbox(toCommbox)
    , _fromCommbox(fromCommbox)
    , _mutex(mutex)
    , _cond(cond)
{

}

std::size_t VisualStream::bytesAvailable()
{
    return _fromCommbox.bytesAvailable();
}

void VisualStream::discardOutBuffer()
{
    _toCommbox.clear();
}

void VisualStream::discardInBuffer()
{
    _fromCommbox.clear();
}


}
}