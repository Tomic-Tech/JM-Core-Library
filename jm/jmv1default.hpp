#ifndef __JM_V1_DEFAULT_HPP__
#define __JM_V1_DEFAULT_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/array.hpp>
#include "jmv1box.hpp"
#include "jmv1shared.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BoxType, typename ProtocolType>
        class Default
        {
        public:
            Default(const boost::shared_ptr<BoxType> &box, 
				const boost::shared_ptr<Shared> &shared, 
				ProtocolType *protocol)
                : _box(box)
                , _shared(shared)
                , _protocol(protocol)
            {

            }

            std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count, bool needRecv)
            {
				boost::array<uint8_t, 255> sendBuff;
                std::size_t length = _protocol->pack(data, count, sendBuff.data(), sendBuff.size());
                _shared->buffID = 0;
                if (_box->newBatch(_shared->buffID))
                {
                    return 0;
                }
                if (length <= 0)
                    return 0;

                if (needRecv)
                {
                    if (!_box->sendOutData(sendBuff.data(), length) ||
                        !_box->runReceive(BoxType::Constant::RECEIVE) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, false))
                    {
                        return 0;
                    }
                }
                else
                {
                    if (!_box->sendOutData(sendBuff.data(), length) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, false))
                    {
                        return 0;
                    }
                }
                _protocol->finishExecute(!needRecv);
                return count;
            }

            gint32 setKeepLink(const boost::uint8_t *data, std::size_t count)
            {
                if (!_box->newBatch(BoxType::Constant::LINKBLOCK))
                {
                    return JM_ERROR_GENERIC;
                }
                if (!_box->sendOutData(data, count) ||
                    !_box->endBatch())
                {
                    return JM_ERROR_GENERIC;
                }
                return JM_ERROR_SUCCESS;
            }
        private:
            boost::shared_ptr<BoxType> _box;
            boost::shared_ptr<Shared> _shared;
            ProtocolType *_protocol;
        };
    }
}

#endif