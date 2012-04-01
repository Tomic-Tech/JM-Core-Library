#include "jmlink.hpp"

namespace JM
{
	Link::Link()
		: _prc(NULL)
	{

	}

	void Link::setProtocol(pointer_t p)
	{
		_prc = p;
	}

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