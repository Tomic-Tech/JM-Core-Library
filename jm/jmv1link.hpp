#ifndef __JM_V1_LINK_HPP__
#define __JM_V1_LINK_HPP__

#include "jmlink.hpp"
#include "jmv1shared.hpp"
#include "jmv1box.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BoxType, typename ProtocolType>
        class Link : public JM::Link
        {
        public:
            Link(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &shared)
                : _box(box)
                , _shared(shared)
                , _protocol(boost::make_shared<ProtocolType>(box, shared))
            {

            }

            std::size_t sendAndRecv(const boost::uint8_t *send, std::size_t count, boost::uint8_t *recv, std::size_t maxLength)
            {
                boost::uint32_t times = 3;
                while (times--)
                {
                    if (sendFrames(send, count) > 0)
                    {
                        return readFrames(recv, maxLength);
                    }
                }
                return 0;
            }

            boost::int32_t setTimeout(boost::int64_t txB2B, boost::int64_t rxB2B, boost::int64_t txF2F, boost::int64_t rxF2F, boost::int64_t total)
            {
                return JM_ERROR_SUCCESS;
            }

            boost::int32_t startKeepLink(bool run)
            {
                if (!_box->keepLink(run))
                    return JM_ERROR_GENERIC;
                return JM_ERROR_SUCCESS;
            }

            std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count)
            {
                return _protocol->sendOneFrame(data, count);
            }

            std::size_t sendFrames(const boost::uint8_t *data, std::size_t count)
            {
                return _protocol->sendFrames(data, count);
            }

            std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength)
            {
                return _protocol->readOneFrame(data, maxLength);
            }

            std::size_t readFrames(boost::uint8_t *data, std::size_t maxLength)
            {
                return _protocol->readFrames(data, maxLength);
            }

            boost::int32_t setKeepLink(const boost::uint8_t *data, std::size_t count)
            {
                return _protocol->setKeepLink(data, count);
            }
        private:
            boost::shared_ptr<BoxType> _box;
            boost::shared_ptr<Shared> _shared;
            boost::shared_ptr<ProtocolType> _protocol;
        };
    }
}

#endif


