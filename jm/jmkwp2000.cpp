#include "jmkwp2000.hpp"
#include "jmutils.h"

namespace JM
{
	KWP2000::KWP2000()
		: _mode(JM_KWP8X)
		, _linkMode(JM_KWP8X)
		, _msgMode(JM_KWP8X)
		, _baud(10416)
		, _targetAddress(0x12)
		, _sourceAddress(0xF1)
	{

	}

	boost::int32_t KWP2000::setOptions(JMKWPMode msgMode, JMKWPMode linkMode, boost::int32_t baud)
	{
		_msgMode = msgMode;
		_linkMode = linkMode;
		_mode = msgMode;
		_baud = baud;

		return JM_ERROR_SUCCESS;
	}

	boost::int32_t KWP2000::setAddress(boost::uint8_t targetAddress, boost::uint8_t sourceAddress)
	{
		_targetAddress = targetAddress;
		_sourceAddress = sourceAddress;

		return JM_ERROR_SUCCESS;
	}

	std::size_t KWP2000::pack(const boost::uint8_t* src, std::size_t srcLength, boost::uint8_t *tar, std::size_t tarLength)
	{
		std::size_t pos = 0;
		boost::uint8_t checksum = 0;
		std::size_t i = 0;

		if (_mode == JM_KWP8X)
		{
			tar[pos++] = JM_LOW_BYTE(0x80 | srcLength);
			tar[pos++] = JM_LOW_BYTE(_targetAddress);
			tar[pos++] = JM_LOW_BYTE(_sourceAddress);
			memcpy(tar + pos, src, srcLength);
			pos += srcLength;
		}
		else if (_mode == JM_KWPCX)
		{
			tar[pos++] = JM_LOW_BYTE(0xC0 | srcLength);
			tar[pos++] = JM_LOW_BYTE(_targetAddress);
			tar[pos++] = JM_LOW_BYTE(_sourceAddress);
			memcpy(tar + pos, src, srcLength);
			pos += srcLength;
		}
		else if (_mode == JM_KWP80)
		{
			tar[pos++] = JM_LOW_BYTE(0x80);
			tar[pos++] = JM_LOW_BYTE(_targetAddress);
			tar[pos++] = JM_LOW_BYTE(_sourceAddress);
			tar[pos++] = JM_LOW_BYTE(srcLength);
			memcpy(tar + pos, src, srcLength);
			pos += srcLength;
		}
		else if (_mode == JM_KWP00)
		{
			tar[pos++] = 0x00;
			tar[pos++] = JM_LOW_BYTE(srcLength);
			memcpy(tar + pos, src, srcLength);
			pos += srcLength;
		}
		else if (_mode == JM_KWPXX)
		{
			tar[pos++] = JM_LOW_BYTE(srcLength);
			memcpy(tar + pos, src, srcLength);
			pos += srcLength;
		}

		for (i = 0; i < pos; ++i)
		{
			checksum += tar[i];
		}
		tar[pos++] = checksum;
		return pos;
	}

	std::size_t KWP2000::unpack(const boost::uint8_t *src, std::size_t srcLength, boost::uint8_t *tar, std::size_t tarLength)
	{
		std::size_t length = 0;

		if (src[0] > 0x80)
		{
			length = src[0] - 0x80;
			if (src[1] != _sourceAddress)
			{
				return 0;
			}
			if (length != (srcLength - KWP8X_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
			{
				length = src[0] - 0xC0; /* For KWPCX */
				if (length != (srcLength - KWPCX_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
				{
					return 0;
				}
			}
			memcpy(tar, src + KWP8X_HEADER_LENGTH, length);
		}
		else if (src[0] == 0x80)
		{
			length = src[0];
			if (src[1] != _sourceAddress)
			{
				return 0;
			}

			if (length != (srcLength - KWP80_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
			{
				return 0;
			}
			memcpy(tar, src + KWP80_HEADER_LENGTH, length);
		}
		else if (src[0] == 0x00)
		{
			length = src[1];
			if (length != (srcLength - KWP00_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
			{
				return 0;
			}
			memcpy(tar, src + KWP00_HEADER_LENGTH, length);
		}
		else
		{
			length = src[0];
			if (length != (srcLength - KWP00_HEADER_LENGTH - KWP_CHECKSUM_LENGTH))
			{
				return 0;
			}
			memcpy(tar, src + KWP00_HEADER_LENGTH, length);
		}
		return length;
	}
}