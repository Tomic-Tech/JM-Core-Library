#ifndef __JM_CANBUS_HPP__
#define __JM_CANBUS_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmcanbusbaud.h>
#include <jm/jmcanbusfiltermask.h>
#include <jm/jmcanbusframetype.h>
#include <jm/jmcanbusidmode.h>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/array.hpp>
#include <jm/jmprotocol.hpp>

namespace JM
{
    class Canbus : public Protocol
    {
    public:
        Canbus();
        virtual ~Canbus();

		virtual std::size_t pack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarMaxLength);
		virtual std::size_t unpack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarMaxLength);

        virtual boost::int32_t setLines(boost::int32_t high, boost::int32_t low);
		virtual boost::int32_t setFilter(const std::vector<boost::int32_t> &idVector);
		virtual boost::int32_t setOptions(boost::int32_t id, JMCanbusBaud baud, JMCanbusIDMode idMode, JMCanbusFilterMask mask, JMCanbusFrameType frame);

    protected:
        boost::int32_t _targetID;
        JMCanbusBaud _baud;
        JMCanbusIDMode _idMode;
        JMCanbusFilterMask _filterMask;
        JMCanbusFrameType _frameType;
        boost::int32_t _high;
        boost::int32_t _low;
        std::vector<boost::int32_t> _idVector;
		boost::array<boost::uint8_t, 8> _flowControl;
    };
}

#endif