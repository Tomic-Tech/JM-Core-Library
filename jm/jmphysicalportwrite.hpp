#ifndef __JM_PHYSICAL_PORT_WRITE_HPP__
#define __JM_PHYSICAL_PORT_WRITE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmphysicalportbase.hpp>
#include <boost/asio/buffer.hpp>

namespace JM
{
	template<typename PortType, typename CommPort>
	class PhysicalPortWrite : public PhysicalPortBase<PortType, CommPort>
	{
	public:
		PhysicalPortWrite(const boost::shared_ptr<PortType> &port)
			: PhysicalPortBase(port)
		{

		}

		void operator()()
		{
			while(true)
			{
				if(!isStop())
				{
					static boost::array<boost::uint8_t, 1024> buff;
					size_t avail = CommPort::inst().outDequeAvailable();
					if (CommPort::inst().popOutDeque(boost::asio::mutable_buffer(buff.data(), avail)))
					{
						port()->write(boost::asio::const_buffer(buff.data(), avail));
					}
				}
				boost::this_thread::yield();
			}
		}
	};
}

#endif