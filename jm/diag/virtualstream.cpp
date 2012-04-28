#include "virtualstream.hpp"

namespace JM
{
namespace Diag
{

VirtualStream::VirtualStream(Stream &toCommbox,
                           Stream &fromCommbox,
                           boost::mutex &mutex,
                           boost::condition_variable &cond)
    : _toCommbox(toCommbox)
    , _fromCommbox(fromCommbox)
    , _mutex(mutex)
    , _cond(cond)
{

}

std::size_t VirtualStream::bytesAvailable()
{
    return _fromCommbox.bytesAvailable();
}

void VirtualStream::discardOutBuffer()
{
    _toCommbox.clear();
}

void VirtualStream::discardInBuffer()
{
    _fromCommbox.clear();
}


}
}