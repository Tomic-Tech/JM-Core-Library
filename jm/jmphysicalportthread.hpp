#ifndef __JM_PHYSICAL_PORT_THREAD_HPP__
#define __JM_PHYSICAL_PORT_THREAD_HPP__

#include <jm/jmphysicalportread.hpp>
#include <jm/jmphysicalportwrite.hpp>

namespace JM
{
	template<typename PortType, typename CommboxPort>
	class PhysicalPortThread
	{
	public:
		PhysicalPortThread()
			: _port(boost::make_shared<PortType>())
			, _read(_port)
			, _write(_port)
			, _readThread(_read)
			, _writeThread(_write)
		{
		}

		void start()
		{
			_read.start();
			_write.start();
		}

		void stop()
		{
			_read.stop();
			_write.stop();
		}

		boost::shared_ptr<PortType> port()
		{
			return _port;
		}
	private:
		boost::shared_ptr<PortType> _port;
		PhysicalPortRead<PortType, CommboxPort> _read;
		PhysicalPortWrite<PortType, CommboxPort> _write;
		boost::thread _readThread;
		boost::thread _writeThread;
	};
}


#endif