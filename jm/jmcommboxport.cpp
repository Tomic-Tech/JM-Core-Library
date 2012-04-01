#include "jmcommboxport.hpp"
#include "jmserialport.h"

namespace JM
{
	CommboxPort::CommboxPort()
		: _readTimeout(boost::posix_time::milliseconds(500))
		, _writeTimeout(boost::posix_time::milliseconds(500))
		, _inDeque()
		, _outDeque()
		, _inMutex()
		, _outMutex()
		, _inCond()
		, _portType(JM_COMMBOX_PORT_UNKNOW)
		, _serialPortThread()
	{
	}

	CommboxPort::~CommboxPort()
	{
		_serialPortThread.stop();
	}

	CommboxPort& CommboxPort::inst()
	{
		static CommboxPort instance;
		return instance;
	}

	void CommboxPort::setType(JMCommboxPortType type)
	{
		_portType = type;
		switch(type)
		{
		case JM_COMMBOX_PORT_SERIAL_PORT:
			_serialPortThread.start();
			break;
		}
	}

	JMCommboxPortType CommboxPort::type()
	{
		return _portType;
	}

	//void CommboxPort::setPointer(void* p)
	//{
	//	if (_portType == JM_COMMBOX_PORT_SERIAL_PORT)
	//	{
	//		_pointer = ((JMSerialPort*)p)->_handle;
	//	}
	//	else
	//	{
	//		_pointer = p;
	//	}
	//}

	//void* CommboxPort::pointer()
	//{
	//	return _pointer;
	//}

	std::size_t CommboxPort::bytesAvailable()
	{
		return _inDeque.size();
	}

	boost::int32_t CommboxPort::discardOutBuffer()
	{
		boost::unique_lock<boost::mutex> lock(_outMutex);
		_outDeque.clear();
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t CommboxPort::discardInBuffer()
	{
		boost::unique_lock<boost::mutex> lock(_inMutex);
		_inDeque.clear();
		return JM_ERROR_SUCCESS;
	}

	std::size_t CommboxPort::outDequeAvailable()
	{
		return _outDeque.size();
	}
}