#include "virtualstream.h"

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

std::size_t VirtualStream::bytes_available()
{
    return _fromCommbox.bytes_available();
}

void VirtualStream::discard_out_buffer()
{
    _toCommbox.clear();
}

void VirtualStream::discard_in_buffer()
{
    _fromCommbox.clear();
}

}
}
