#ifndef __JM_PHYSICAL_PORT_BASE_HPP__
#define __JM_PHYSICAL_PORT_BASE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/smart_ptr.hpp>

namespace JM
{
	template<typename PortType, typename CommPort>
	class PhysicalPortBase
	{
	public:
		PhysicalPortBase(const boost::shared_ptr<PortType> &port)
			: _port(port)
			, _isStop(boost::make_shared<bool>(true))
			, _mutex(boost::make_shared<boost::mutex>())
		{

		}

		~PhysicalPortBase()
		{
			stop();
		}

		bool isStop()
		{
			boost::mutex::scoped_lock lock(*_mutex);
			return *_isStop;
		}

		boost::shared_ptr<PortType> port()
		{
			return _port;
		}

		void stop()
		{
			boost::mutex::scoped_lock lock(*_mutex);
			*_isStop = true;
		}

		void start()
		{
			boost::mutex::scoped_lock lock(*_mutex);
			*_isStop = false;
		}
	private:
		boost::shared_ptr<PortType> _port;
		boost::shared_ptr<bool> _isStop;
		boost::shared_ptr<boost::mutex> _mutex;
	};
}

#endif