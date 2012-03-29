#ifndef __JM_KWP2000_HPP__
#define __JM_KWP2000_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmkwpmode.h>
#include <jm/jmerror.h>
#include <boost/cstdint.hpp>
#include <jm/jmprotocol.hpp>

namespace JM
{
    class KWP2000 : Protocol
    {
    public:
        static const boost::uint8_t KWP8X_HEADER_LENGTH = 3;
        static const boost::uint8_t KWPCX_HEADER_LENGTH = 3;
        static const boost::uint8_t KWP80_HEADER_LENGTH = 4;
        static const boost::uint8_t KWPXX_HEADER_LENGTH = 1;
        static const boost::uint8_t KWP00_HEADER_LENGTH = 2;
        static const boost::uint8_t KWP_CHECKSUM_LENGTH = 1;
        static const boost::uint16_t KWP_MAX_DATA_LENGTH = 128;
    public:
        KWP2000();
        
        virtual boost::int32_t setOptions(JMKWPMode msgMode, JMKWPMode linkMode, boost::int32_t baud);
        virtual boost::int32_t setAddress(boost::uint8_t targetAddress, boost::uint8_t sourceAddress);
		virtual std::size_t pack(const boost::uint8_t* src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength);
		virtual std::size_t unpack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength);

        virtual boost::int32_t addrInit(boost::uint8_t addrCode) = 0;
        virtual boost::int32_t fastInit(const boost::uint8_t *data, std::size_t count) = 0;
        virtual boost::int32_t setLines(boost::int32_t comLine, bool lLine) = 0;
    protected:
        JMKWPMode _mode;
        JMKWPMode _linkMode;
        JMKWPMode _msgMode;
        boost::int32_t _baud;
        boost::uint32_t _targetAddress;
        boost::uint32_t _sourceAddress;

    };
}

#endif