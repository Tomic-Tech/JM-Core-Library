#ifndef __JM_V1_BOX_H__
#define __JM_V1_BOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/smart_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include "jmcommboxport.hpp"
#include "jmv1shared.hpp"
#include "jmv1boxw80c.hpp"
#include "jmv1boxc168c.hpp"

namespace JM
{
	namespace V1
	{
		template<typename ConstantClass>
		class Box
		{
		public:
			typedef ConstantClass Constant;
			typedef boost::posix_time::microseconds MicroSeconds;
			typedef boost::posix_time::milliseconds MilliSeconds;
			typedef boost::posix_time::seconds Seconds;
			typedef boost::asio::mutable_buffer MutableBuffer;
			typedef boost::asio::const_buffer ConstBuffer;
		public:
			Box(const boost::shared_ptr<Shared> &shared)
				: _shared(shared)
			{
			}
			virtual ~Box()
			{
			}

			static inline JM::CommboxPort &commboxPort()
			{
				return JM::CommboxPort::inst();
			}

			template<typename DurationType>
			static inline boost::int64_t toMicroSeconds(DurationType const &duration)
			{
				return duration.total_microseconds();
			}

			template<typename DurationType>
			static inline void sleep(DurationType const &duration)
			{
				boost::this_thread::sleep(duration);
			}

			void getLinkTime(boost::uint8_t type, boost::int64_t time)
			{
				if (type == Constant::SETBYTETIME)
				{
					_shared->reqByteToByte = time;
				}
				else if (type == Constant::SETRECBBOUT)
				{
					_shared->reqWaitTime = time;
				}
				else if (type == Constant::SETRECFROUT)
				{
					_shared->resByteToByte = time;
				}
				else
				{
					_shared->resWaitTime = time;
				}
			}

			virtual bool openComm() = 0;
			virtual bool closeComm() = 0;
			virtual std::size_t readBytes(boost::uint8_t *buff, std::size_t count) = 0;
			virtual bool setCommCtrl(boost::uint8_t valueOpen, boost::uint8_t valueClose) = 0;
			virtual bool setCommLine(boost::uint8_t sendLine, boost::uint8_t recvLine) = 0;
			virtual bool setCommLink(boost::uint8_t ctrlWord1, boost::uint8_t ctrlWord2, boost::uint8_t ctrlWord3) = 0;
			virtual bool setCommBaud(boost::uint32_t baud) = 0;
			virtual bool setCommTime(boost::uint8_t type, boost::int64_t microseconds) = 0;
			virtual bool setLineLevel(boost::uint8_t valueLow, boost::uint8_t valueHigh) = 0;
			virtual bool commboxDelay(boost::int64_t microseconds) = 0;
			virtual bool turnOverOneByOne() = 0;
			virtual bool stopNow(bool isStopExecute) = 0;
			virtual bool newBatch(boost::uint8_t buffID) = 0;
			virtual bool delBatch(boost::uint8_t buffID) = 0;
			virtual bool checkResult(boost::int64_t microseconds) = 0;
			virtual bool keepLink(bool isRunLink) = 0;
			virtual bool sendOutData(const boost::uint8_t *buff, std::size_t count) = 0;
			virtual bool runReceive(boost::uint8_t type) = 0;
			virtual bool endBatch() = 0;
			virtual bool runBatch(boost::uint8_t *buffID, std::size_t count, bool isExecuteMany) = 0;
			virtual std::size_t readData(boost::uint8_t *buff, std::size_t count, boost::int64_t microseconds) = 0;
			virtual bool updateBuff(boost::uint8_t type, boost::uint8_t *buff) = 0;
			virtual boost::uint8_t getAbsAdd(boost::uint8_t buffID, boost::uint8_t add) = 0;

		protected:
			boost::shared_ptr<Shared> _shared;
		};
	}
}

#endif