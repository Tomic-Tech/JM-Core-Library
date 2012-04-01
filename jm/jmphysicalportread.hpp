#ifndef __JM_PHYSICAL_PORT_READ_HPP__
#define __JM_PHYSICAL_PORT_READ_HPP__

#include <jm/jmphysicalportbase.hpp>

namespace JM
{
	template<typename PortType, typename CommPort>
	class PhysicalPortRead : public PhysicalPortBase<PortType, CommPort>
	{
	public:
		PhysicalPortRead(const boost::shared_ptr<PortType> &port)
			: PhysicalPortBase<PortType, CommPort>(port)
		{

		}

		void operator()()
		{
			while (true)
			{
				if(!isStop())
				{
					static boost::array<boost::uint8_t, 1024> buff;
					static size_t avail = 0;

					avail = port()->bytesAvailable();
					if (avail)
					{
						port()->read(boost::asio::mutable_buffer(buff.data(), avail));
						CommPort::inst().pushInDeque(boost::asio::const_buffer(buff.data(), avail));
					}
				}
				boost::this_thread::yield();
			}
		}
	};

}

#endif