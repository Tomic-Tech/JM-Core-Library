#ifndef __JM_MIKUNI_HPP__
#define __JM_MIKUNI_HPP__

#include <boost/cstdint.hpp>
#include <boost/asio/buffer.hpp>
#include <jm/jmprotocol.hpp>

namespace JM
{
    class Mikuni : public Protocol
    {
    public:
        static const boost::uint8_t HEAD_FORMAT = 0x48;
    public:
        Mikuni();
		virtual std::size_t pack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength);
		virtual std::size_t unpack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength);

        virtual boost::int32_t init() = 0;
    };
}

#endif