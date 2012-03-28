#include "jmkwp1281.hpp"
#include "jmutils.h"

namespace JM
{
	KWP1281::KWP1281()
		: _frameCounter(0)
	{

	}

	KWP1281::~KWP1281()
	{

	}

	guint8 KWP1281::frameCounterIncrement()
	{
		return ++(_frameCounter);
	}

	std::size_t KWP1281::pack(const boost::uint8_t *src, std::size_t srcLength, boost::uint8_t *tar, std::size_t tarLength)
	{
		tar[0] = JM_LOW_BYTE(srcLength + 20);
		tar[1] = frameCounterIncrement();

		memcpy(tar + 2, src, srcLength);
		tar[srcLength + 2] = FRAME_END;
		return srcLength + 3;
	}

	std::size_t KWP1281::unpack(const boost::uint8_t *src, std::size_t srcLength, boost::uint8_t *tar, std::size_t tarLength)
	{
		memcpy(tar, src + 1, srcLength - 2);
		return srcLength - 2;
	}
}