#include "jmmikuni.hpp"

namespace JM
{
	Mikuni::Mikuni()
	{

	}

	std::size_t Mikuni::pack(const boost::uint8_t *src, std::size_t srcLength, boost::uint8_t *tar, std::size_t tarLength)
	{
		if (srcLength <= 0)
			return 0;

		if ((srcLength + 3) > tarLength)
			return 0;

		tar[0] = HEAD_FORMAT;
		memcpy(tar + 1, src, srcLength);
		tar[srcLength + 1] = 0x0D;
		tar[srcLength + 2] = 0x0A;
		return srcLength + 3;
	}

	std::size_t Mikuni::unpack(const boost::uint8_t *src, std::size_t srcLength, boost::uint8_t *tar, std::size_t tarLength)
	{
		if (srcLength <= 0)
			return 0;

		if ((srcLength - 3) > tarLength)
			return 0;

		memcpy(tar, src + 1, srcLength - 3);
		return srcLength - 3;
	}
}