#ifndef __JM_DIAG_BOX_STREAM_H__
#define __JM_DIAG_BOX_STREAM_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/virtualstream.h>
#include <jm/diag/realstream.h>
#include <jm/asio/serialport.h>

namespace JM
{
namespace Diag
{

class BoxStream : public boost::noncopyable
{
public:
    enum RealType
    {
        SerialPort,
        Unknow
    };
public:
    static BoxStream &inst();
    JMCORE_API void set_type(RealType t);
    RealType type();
    JM::Asio::serial_port_ptr serial_port();
    VirtualStreamPtr vs();
private:
    BoxStream();
    ~BoxStream();
private:
    Stream _toCommbox;
    Stream _fromCommbox;
    boost::mutex _mutex;
    boost::condition_variable _cond;
    boost::shared_ptr<RealStream<JM::Asio::serial_port> > _serialPortStream;
    VirtualStreamPtr _virtualStream;
    RealType _type;
};

}
}

#endif /* __JM_DIAG_BOXSTREAM_H__ */
