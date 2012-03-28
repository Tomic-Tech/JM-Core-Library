#ifndef __JM_PROTOCOL_HPP__
#define __JM_PROTOCOL_HPP__

#include <boost/asio/buffer.hpp>

namespace JM
{
	class Protocol
	{
	public:
		virtual std::size_t pack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength) = 0;
		virtual std::size_t unpack(const boost::uint8_t *src,
			std::size_t srcLength,
			boost::uint8_t *tar,
			std::size_t tarLength) = 0;

		template<typename ConstBufferSequence, typename MutableBufferSequence>
		std::size_t pack(const ConstBufferSequence &src, const MutableBufferSequence &tar)
		{
			return pack(boost::asio::buffer_cast<const boost::uint8_t*>(src),
				boost::asio::buffer_size(src),
				boost::asio::buffer_cast<boost::uint8_t*>(tar),
				boost::asio::buffer_size(tar));
		}

		template<typename ConstBufferSequence, typename MutableBufferSequence>
		std::size_t unpack(const ConstBufferSequence &src, const MutableBufferSequence &tar)
		{
			return unpack(boost::asio::buffer_cast<const boost::uint8_t*>(src),
				boost::asio::buffer_size(src),
				boost::asio::buffer_cast<boost::uint8_t*>(tar),
				boost::asio::buffer_size(tar));
		}
	};
}

#endif