#ifndef __JM_KWP1281_HPP__
#define __JM_KWP1281_HPP__

#include <jm/jmutils.h>
#include <boost/cstdint.hpp>
#include <boost/asio/buffer.hpp>
#include <jm/jmprotocol.hpp>

namespace JM
{
    class KWP1281 : public Protocol
    {
    public:
        static const boost::uint8_t FRAME_END = 0x03;
    public:
        KWP1281();
		virtual ~KWP1281();
		virtual std::size_t pack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength);
		virtual std::size_t unpack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength);

        virtual boost::int32_t addrInit(boost::uint8_t addrCode) = 0;
        virtual boost::int32_t setLines(boost::int32_t comLine, bool lLine) = 0;

    private:
        boost::uint8_t frameCounterIncrement();

    protected:
        boost::uint8_t _frameCounter;
    };
}

#endif