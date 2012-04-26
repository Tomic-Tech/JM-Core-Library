#ifndef __JM_DIAG_V1_DEFAULT_HPP__
#define __JM_DIAG_V1_DEFAULT_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/array.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
template<typename BoxType, typename P>
class Default
{
public:
    Default(const boost::shared_ptr<BoxType> &box,
            const boost::shared_ptr<Shared> &shared,
            P *pro)
        : _box(box)
        , _shared(shared)
        , _pro(pro)
    {

    }
    std::size_t sendOneFrame(const boost::uint8_t *data,
                             std::size_t count,
                             bool needRecv,
                             boost::system::error_code &ec)
    {
        boost::array<uint8_t, 255> sendBuff;
        std::size_t length = _pro->pack(data, count, sendBuff.data(),
                                             sendBuff.size(), ec);

        if (ec)
        {
            return 0;
        }

        _shared->buffID = 0;
        if (_box->newBatch(_shared->buffID))
        {
            ec = boost::asio::error::connection_refused;
            return 0;
        }
        if (length <= 0)
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        if (needRecv)
        {
            if (!_box->sendOutData(sendBuff.data(), length) ||
                    !_box->runReceive(BoxType::Constant::RECEIVE) ||
                    !_box->endBatch() ||
                    !_box->runBatch(&_shared->buffID, 1, false))
            {
                ec = boost::asio::error::connection_refused;
                return 0;
            }
        }
        else
        {
            if (!_box->sendOutData(sendBuff.data(), length) ||
                    !_box->endBatch() ||
                    !_box->runBatch(&_shared->buffID, 1, false))
            {
                ec = boost::asio::error::connection_refused;
                return 0;
            }
        }
        ec = boost::system::error_code();
        _pro->finishExecute(!needRecv);
        return count;
    }

    void setKeepLink(const boost::uint8_t *data,
                     std::size_t count,
                     boost::system::error_code &ec)
    {
        if (!_box->newBatch(BoxType::Constant::LINKBLOCK))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }
        if (!_box->sendOutData(data, count) ||
                !_box->endBatch())
        {
            ec = boost::asio::error::connection_refused;
            return;
        }
        ec = boost::system::error_code();
    }
private:
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    P *_pro;
};
}
}
}

#endif