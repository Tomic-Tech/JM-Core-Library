#ifndef __JM_COMMBOX_PORT_HPP__
#define __JM_COMMBOX_PORT_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmcommboxporttype.h"
#include <deque>
#include <boost/cstdint.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "jmserialport.hpp"

namespace JM
{
    class CommboxPort
    {
    public:
        static CommboxPort& inst();
        void setType(JMCommboxPortType type);
        JMCommboxPortType type();
        void setPointer(pointer_t p);
        pointer_t pointer();

		template<typename DurationType>
        boost::int32_t setReadTimeout(DurationType const &time)
		{
			_readTimeout = time;
			return JM_ERROR_SUCCESS;
		}

		template<typename DurationType>
        boost::int32_t setWriteTimeout(DurationType const &time)
		{
			_writeTimeout = time;
			return JM_ERROR_SUCCESS;
		}

        std::size_t bytesAvailable();
        boost::int32_t discardOutBuffer();
        boost::int32_t discardInBuffer();

		template<typename MutableBufferSequence>
        std::size_t read(const MutableBufferSequence &data)
		{
			std::size_t count = boost::asio::buffer_size(data);
			if (count <= _inDeque.size())
				return readImmediately(data);

			if (_readTimeout.total_microseconds() <= 0)
				return readWithoutTimeout(data);

			return readWithTimeout(data);
		}

		template<typename ConstBufferSequence>
        std::size_t write(const ConstBufferSequence &data)
		{
			std::size_t count = boost::asio::buffer_size(data);
			const boost::uint8_t *p = boost::asio::buffer_cast<const boost::uint8_t*>(data);
			boost::unique_lock<boost::mutex> lock(_outMutex);
			for (std::size_t i = 0; i < count; i++)
			{
				_outDeque.push_back(p[i]);
			}
			return boost::asio::buffer_size(data);
		}

		template<typename ConstBufferSequence>
        void pushInDeque(const ConstBufferSequence &data)
		{
			boost::unique_lock<boost::mutex> lock(_inMutex);
			const boost::uint8_t* p = boost::asio::buffer_cast<const boost::uint8_t*>(data);
			std::size_t count = boost::asio::buffer_size(data);
			for (std::size_t i = 0; i < count; i++)
			{
				_inDeque.push_back(p[i]);
			}
		}

        std::size_t outDequeAvailable();

		template<typename MutableBufferSequence>
        bool popOutDeque(const MutableBufferSequence &data)
		{
			std::size_t count = boost::asio::buffer_size(data);
			boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);
			boost::unique_lock<boost::mutex> lock(_outMutex);
			if (_outDeque.empty() || _outDeque.size() < count)
				return false;

			for (std::size_t i = 0; i < count; i++)
			{
				p[i] = _outDeque.front();
				_outDeque.pop_front();
			}

			return true;
		}

    private:
        CommboxPort();
        ~CommboxPort();

		template<typename MutableBufferSequence>
        std::size_t readImmediately(const MutableBufferSequence &data)
		{
			std::size_t count = boost::asio::buffer_size(data);
			boost::unique_lock<boost::mutex> lock(_inMutex);
			boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);
			for (std::size_t i = 0; i < count; i++)
			{
				p[i] = _inDeque.front();
				_inDeque.pop_front();
			}
			return count;
		}

		template<typename MutableBufferSequence>
        std::size_t readWithoutTimeout(const MutableBufferSequence &data)
		{
			std::size_t count = boost::asio::buffer_size(data);
			boost::unique_lock<boost::mutex> lock(_inMutex);
			boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);
			while (_inDeque.size() < count)
			{
				_inCond.wait(lock);
			}
			for (std::size_t i = 0; i < count; i++)
			{
				p[i] = _inDeque.front();
				_inDeque.pop_front();
			}
			return count;
		}

		template<typename MutableBufferSequence>
        std::size_t readWithTimeout(const MutableBufferSequence &data)
		{
			std::size_t count = boost::asio::buffer_size(data);
			boost::unique_lock<boost::mutex> lock(_inMutex);
			boost::uint8_t *p = boost::asio::buffer_cast<boost::uint8_t*>(data);
			size_t len = 0;
			while (_inCond.timed_wait(lock, _readTimeout))
			{
				std::size_t size = _inDeque.size();
				if (size >= count)
				{
					for (std::size_t i = 0; i < count; i++)
					{
						p[i + len] = _inDeque.front();
						_inDeque.pop_front();
					}
					len += count;
					break;
				}
				for (std::size_t i = 0; i < size; i++)
				{
					p[i + len] = _inDeque.front();
					_inDeque.pop_front();
				}
				len += size;
				count -= size;
			}
			return len;
		}
    private:
        boost::posix_time::time_duration _readTimeout;
        boost::posix_time::time_duration _writeTimeout;
		std::deque<boost::uint8_t> _inDeque;
		std::deque<boost::uint8_t> _outDeque;
		boost::mutex _inMutex;
		boost::mutex _outMutex;
		boost::condition_variable _inCond;
        JMCommboxPortType _portType;
        pointer_t _pointer;
    };
}

#endif