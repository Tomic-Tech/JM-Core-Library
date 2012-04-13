#ifndef __JM_DIAG_COMMBOX_HPP__
#define __JM_DIAG_COMMBOX_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <hash_map>
#include <boost/smart_ptr.hpp>
#include <jm/diag/physicalstream.hpp>
#include <jm/diag/virsualstream.hpp>

namespace JM
{
namespace Diag
{
class Commbox : public boost::noncopyable
{
public:
    enum Type
    {
        SerialPort,
        Unknow
    };
public:
    static Commbox& inst();
    JMCORE_API void setType(Type type);
    Type type();

    template<typename PhysicalType>
    PhysicalStream<PhysicalType>* physicalStream()
    {
        void * ss = NULL;
        switch(_type)
        {
        case SerialPort:
            if (_physicalStreams.find(_type) != _physicalStreams.end())
            {
                ss = _physicalStreams[_type];
            }
            if (ss != NULL)
                break;
            ss = new PhysicalStream<boost::asio::serial_port>(_toCommbox,
                    _fromCommbox,
                    _mutex,
                    _cond);
            _physicalStreams[_type] = ss;
            break;
        default:
            break;
        }

        if (ss != NULL)
            return (PhysicalStream<PhysicalType>*)_physicalStreams[_type];

        else
            return NULL;
    }

    boost::shared_ptr<VisualStream> visualStream();
private:
    Commbox();
    ~Commbox();
private:
    Stream _toCommbox;
    Stream _fromCommbox;
    boost::mutex _mutex;
    boost::condition_variable _cond;
    std::hash_map<Type, void* > _physicalStreams;
    boost::shared_ptr<VisualStream> _visualStream;
    Type _type;
};
}
}

#endif