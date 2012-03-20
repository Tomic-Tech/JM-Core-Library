#ifndef __JM_V1_LINK_HPP__
#define __JM_V1_LINK_HPP__

#include "jmlink.hpp"
#include "jmv1shared.hpp"
#include "jmv1box.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BOX, typename PROTOCOL>
        class Link : public JM::Link
        {
        public:
            Link(BOX *box, Shared *shared)
                : _box(box)
                , _shared(shared)
                , _protocol(box, shared)
            {

            }

            size_t sendAndRecv(const guint8 *send, size_t count, guint8 *recv)
            {
                guint32 times = 3;
                while (times--)
                {
                    if (sendFrames(send, count) > 0)
                    {
                        return readFrames(recv);
                    }
                }
                return 0;
            }

            gint32 setTimeout(gint32 txB2B, gint32 rxB2B, gint32 txF2F, gint32 rxF2F, gint32 total)
            {
                return JM_ERROR_SUCCESS;
            }

            gint32 startKeepLink(gboolean run)
            {
                if (!_box->keepLink(run))
                    return JM_ERROR_GENERIC;
                return JM_ERROR_SUCCESS;
            }

            size_t sendOneFrame(const guint8 *data, size_t count)
            {
                return _protocol.sendOneFrame(data, count);
            }

            size_t sendFrames(const guint8 *data, size_t count)
            {
                return _protocol.sendFrames(data, count);
            }

            size_t readOneFrame(guint8 *data)
            {
                return _protocol.readOneFrame(data);
            }

            size_t readFrames(guint8 *data)
            {
                return _protocol.readFrames(data);
            }

            gint32 setKeepLink(const guint8 *data, size_t count)
            {
                return _protocol.setKeepLink(data, count);
            }
        private:
            BOX *_box;
            Shared *_shared;
            PROTOCOL _protocol;
        };
    }
}

#endif


