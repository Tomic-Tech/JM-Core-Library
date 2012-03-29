#include "jmlink.hpp"

namespace JM
{
	pointer_t Link::protocol()
	{
		return _prc;
	}

	void Link::setProtocolType(JMProtocolType type)
	{
		_prcType = type;
	}

	JMProtocolType Link::protocolType()
	{
		return _prcType;
	}
}