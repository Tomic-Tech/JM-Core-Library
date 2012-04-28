#ifndef __JM_DIAG_V1_BOX_LINK_HPP__
#define __JM_DIAG_V1_BOX_LINK_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/boxlink.hpp>
#include <jm/diag/v1/shared.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
template<typename BoxType, typename P>
class BoxLink : public Diag::BoxLink
{
public:
    BoxLink(const boost::shared_ptr<BoxType> &box,
            const boost::shared_ptr<Shared> &shared)
        : _box(box)
        , _shared(shared)
        , _protocol(boost::make_shared<P>(box, shared))
    {
        setProtocol(_protocol.get());
    }

    std::size_t sendAndRecv(const boost::uint8_t *send,
                            std::size_t count,
                            boost::uint8_t *recv,
                            std::size_t maxLength,
                            boost::system::error_code &ec)
    {
        boost::uint32_t times = 3;
        while (times--)
        {
            sendFrames(send, count, ec);
            if (ec)
            {
                continue;
            }
            return readFrames(recv, maxLength, ec);
        }
        return 0;
    }

    void setTimeout(boost::int64_t txB2B,
                    boost::int64_t rxB2B,
                    boost::int64_t txF2F,
                    boost::int64_t rxF2F,
                    boost::int64_t total,
                    boost::system::error_code &ec)
    {
        ec = boost::system::error_code();
    }

    void startKeepLink(bool run, boost::system::error_code &ec)
    {
        if (!_box->keepLink(run))
        {
            ec = boost::asio::error::connection_refused;
        }
        else
        {
            ec = boost::system::error_code();
        }
    }

    std::size_t sendOneFrame(const boost::uint8_t *data,
                             std::size_t count,
                             boost::system::error_code &ec)
    {
        return _protocol->sendOneFrame(data, count, ec);
    }

    std::size_t sendFrames(const boost::uint8_t *data,
                           std::size_t count,
                           boost::system::error_code &ec)
    {
        return _protocol->sendFrames(data, count, ec);
    }

    std::size_t readOneFrame(boost::uint8_t *data,
                             std::size_t maxLength,
                             boost::system::error_code &ec)
    {
        return _protocol->readOneFrame(data, maxLength, ec);
    }

    std::size_t readFrames(boost::uint8_t *data,
                           std::size_t maxLength,
                           boost::system::error_code &ec)
    {
        return _protocol->readFrames(data, maxLength, ec);
    }

    void setKeepLink(const boost::uint8_t *data,
                     std::size_t count,
                     boost::system::error_code &ec)
    {
        _protocol->setKeepLink(data, count, ec);
    }
protected:
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<P> _protocol;
};
}
}
}

#endif
