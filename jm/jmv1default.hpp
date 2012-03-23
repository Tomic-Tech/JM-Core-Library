#ifndef __JM_V1_DEFAULT_HPP__
#define __JM_V1_DEFAULT_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmv1box.hpp"
#include "jmv1shared.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BOX, typename PROTOCOL>
        class Default
        {
        public:
            Default(BOX *box, Shared *shared, PROTOCOL *protocol)
                : _box(box)
                , _shared(shared)
                , _protocol(protocol)
            {

            }

            size_t sendOneFrame(const guint8 *data, size_t count, gboolean needRecv)
            {
                guint8 sendBuff[256];
                size_t length = _protocol->pack(data, count, sendBuff);
                _shared->buffID = 0;
                if (_box->newBatch(_shared->buffID))
                {
                    return 0;
                }
                if (length <= 0)
                    return 0;

                if (needRecv)
                {
                    if (!_box->sendOutData(sendBuff, length) ||
                        !_box->runReceive(BOX::C::RECEIVE) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, FALSE))
                    {
                        return 0;
                    }
                }
                else
                {
                    if (!_box->sendOutData(sendBuff, length) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, FALSE))
                    {
                        return 0;
                    }
                }
                _protocol->finishExecute(!needRecv);
                return count;
            }

            gint32 setKeepLink(const guint8 *data, size_t count)
            {
                if (!_box->newBatch(BOX::C::LINKBLOCK))
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
            BOX *_box;
            Shared *_shared;
            PROTOCOL *_protocol;
        };
    }
}

#endif