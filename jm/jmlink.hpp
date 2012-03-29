#ifndef __JM_LINK_HPP__
#define __JM_LINK_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmprotocoltype.h"
#include <boost/cstdint.hpp>

namespace JM
{
    class Link
    {
    public:

        virtual pointer_t protocol();
        void setProtocolType(JMProtocolType type);
        JMProtocolType protocolType();

		virtual std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t length) = 0;
		virtual std::size_t sendFrames(const boost::uint8_t *data, std::size_t length) = 0;
		virtual std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength) = 0;
		virtual std::size_t readFrames(boost::uint8_t *data, std::size_t maxLength) = 0;
		virtual std::size_t sendAndRecv(const boost::uint8_t *send,
			std::size_t sendLenth,
			boost::uint8_t *recv,
			std::size_t recvMaxLength) = 0;
		virtual boost::int32_t startKeepLink(bool run) = 0;
		virtual boost::int32_t setKeepLink(const boost::uint8_t *data, std::size_t length) = 0;
		// All parameter unit is microseconds.
		virtual boost::int32_t setTimeout(boost::int64_t txB2B, 
			boost::int64_t rxB2B, 
			boost::int64_t txF2F, 
			boost::int64_t rxF2F, 
			boost::int64_t total) = 0;

		template<typename ConstBufferSequence>
        std::size_t sendOneFrame(const ConstBufferSequence &data)
		{
			return sendOneFrame(boost::asio::buffer_cast<const boost::uint8_t*>(data),
				boost::asio::buffer_size(data));
		}

		template<typename ConstBufferSequence>
        std::size_t sendFrames(const ConstBufferSequence &data)
		{
			return sendFrames(boost::asio::buffer_cast<const boost::uint8_t*>(data),
				boost::asio::buffer_size(data));
		}

		template<typename MutableBufferSequence>
        std::size_t readOneFrame(const MutableBufferSequence &data)
		{
			return readOneFrame(boost::asio::buffer_cast<boost::uint8_t *>(data), boost::asio::buffer_size(data));
		}

		template<typename MutableBufferSequence>
        std::size_t readFrames(const MutableBufferSequence &data)
		{
			return readOneFrame(boost::asio::buffer_cast<boost::uint8_t *>(data),
				boost::asio::buffer_size(data));
		}

		template<typename ConstBufferSequence, typename MutableBufferSequence>
        std::size_t sendAndRecv(const ConstBufferSequence &send, const MutableBufferSequence &recv)
		{
			return sendAndRecv(boost::asio::buffer_cast<const boost::uint8_t*>(send),
				boost::asio::buffer_size(send),
				boost::asio::buffer_cast<boost::uint8_t*>(recv),
				boost::asio::buffer_size(recv));
		}

		template<typename ConstBufferSequence>
        boost::int32_t setKeepLink(const ConstBufferSequence &data)
		{
			return setKeepLink(boost::asio::buffer_cast<const boost::uint8_t*>(data),
				boost::asio::buffer_size(data));
		}

		template<typename DurationType1, typename DurationType2, typename DurationType3, typename DurationType4, typename DurationType5>
		boost::int32_t setTimeout(DurationType1 const &txB2B, 
			DurationType2 const &rxB2B, 
			DurationType3 const &txF2F, 
			DurationType4 const &rxF2F, 
			DurationType5 const &total)
		{
			return setTimeout(txB2B.total_microseconds(),
				rxB2B.total_microseconds(),
				txF2F.total_microseconds(),
				rxF2F.total_microseconds(),
				total.total_microseconds());
		}

    private:
        JMProtocolType _prcType;
        pointer_t _prc;
    };
}

#endif