#include "jmcanbus.hpp"
#include "jmutils.h"
#include "jmerror.h"

namespace JM
{
	Canbus::Canbus()
	{
		_flowControl[0] = 0x30;
		_flowControl[1] = 0x00;
		_flowControl[2] = 0x00;
		_flowControl[3] = 0x00;
		_flowControl[4] = 0x00;
		_flowControl[5] = 0x00;
		_flowControl[6] = 0x00;
		_flowControl[7] = 0x00;
	}

	Canbus::~Canbus()
	{

	}

	std::size_t Canbus::pack(const boost::uint8_t *src,
		std::size_t srcLength,
		boost::uint8_t *tar,
		std::size_t tarMaxLength)
	{
		if (srcLength > 8 || srcLength <= 0)
		{
			return 0;
		}

		if (_idMode == JM_CANBUS_ID_MODE_STD)
		{
			tar[1] = JM_HIGH_BYTE(JM_LOW_WORD(_targetID));
			tar[2] = JM_LOW_BYTE(JM_LOW_WORD(_targetID));
			if (_frameType == JM_CANBUS_FRAME_TYPE_DATA)
			{
				tar[0] = JM_LOW_BYTE(srcLength | JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA);
			}
			else
			{
				tar[0] = JM_LOW_BYTE(srcLength | JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_REMOTE);
			}
			memcpy(tar + 3, src, srcLength);
			return srcLength + 3;
		}
		if (_idMode == JM_CANBUS_ID_MODE_EXT)
		{
			tar[1] = JM_HIGH_BYTE(JM_HIGH_WORD(_targetID));
			tar[2] = JM_LOW_BYTE(JM_HIGH_WORD(_targetID));
			tar[3] = JM_HIGH_BYTE(JM_LOW_WORD(_targetID));
			tar[4] = JM_LOW_BYTE(JM_LOW_WORD(_targetID));
			if (_frameType == JM_CANBUS_FRAME_TYPE_DATA)
			{
				tar[0] = JM_LOW_BYTE(srcLength | JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA);
			}
			else
			{
				tar[0] = JM_LOW_BYTE(srcLength | JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE);
			}
			memcpy(tar + 5, src, srcLength);
			return srcLength + 5;
		}
		return 0;
	}

	std::size_t Canbus::unpack(const boost::uint8_t *src,
		std::size_t srcLength,
		boost::uint8_t *tar,
		std::size_t tarMaxLength)
	{
		if (srcLength <= 0)
		{
			return 0;
		}

		std::size_t length = 0;

		boost::uint32_t mode = (src[0] & (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE));
		if (mode == (JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA))
		{
			length = src[0] & 0x0F;
			if (length != srcLength - 3)
			{
				return 0;
			}

			memcpy(tar, src + 3, length);
		}

		if (mode == (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA))
		{
			length = src[0] & 0x0F;
			if (length != srcLength - 5)
			{
				return 0;
			}

			memcpy(tar, src + 5, length);
		}
		return length;
	}

	boost::int32_t Canbus::setLines(boost::int32_t high, boost::int32_t low)
	{
		_high = high;
		_low = low;

		return JM_ERROR_SUCCESS;
	}

	boost::int32_t Canbus::setFilter(const std::vector<boost::int32_t> &idVector)
	{
		_idVector = idVector;
		return JM_ERROR_SUCCESS;
	}

	boost::int32_t Canbus::setOptions(boost::int32_t id, JMCanbusBaud baud, JMCanbusIDMode idMode, JMCanbusFilterMask mask, JMCanbusFrameType frame)
	{
		_baud = baud;
		_idMode = idMode;
		_filterMask = mask;
		_frameType = frame;
		_targetID = id;
		return JM_ERROR_SUCCESS;
	}
}