#ifndef __JM_DIAG_V1_DEFAULT_FUNC_H__
#define __JM_DIAG_V1_DEFAULT_FUNC_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/v1/shared.h>

namespace JM
{
namespace Diag
{
namespace V1
{

template<typename BoxType, typename P>
class DefaultFunc
{
public:

    DefaultFunc(const boost::shared_ptr<BoxType> &box,
                 const boost::shared_ptr<Shared> &s,
                 P *pro)
    : _box(box)
    , _shared(s)
    , _pro(pro)
    {

    }

    std::size_t send_one_frame(const boost::uint8_t *data,
                               std::size_t count,
                               bool needRecv,
                               boost::system::error_code &ec)
    {
        boost::array<uint8_t, 255 > sendBuff;
        std::size_t length = _pro->pack(data, count, sendBuff.data(),
                                        sendBuff.size(), ec);

        if (ec)
        {
            return 0;
        }

        _shared->buffID = 0;
        if (_box->new_batch(_shared->buffID))
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
            if (!_box->send_out_data(sendBuff.data(), length) ||
                    !_box->run_receive(BoxType::Constant::RECEIVE) ||
                    !_box->end_batch() ||
                    !_box->run_batch(&_shared->buffID, 1, false))
            {
                ec = boost::asio::error::connection_refused;
                return 0;
            }
        }
        else
        {
            if (!_box->send_out_data(sendBuff.data(), length) ||
                    !_box->end_batch() ||
                    !_box->run_batch(&_shared->buffID, 1, false))
            {
                ec = boost::asio::error::connection_refused;
                return 0;
            }
        }
        ec = boost::system::error_code();
        _pro->finish_execute(!needRecv);
        return count;
    }

    boost::system::error_code set_keep_link(const boost::uint8_t *data,
                                            std::size_t count,
                                            boost::system::error_code &ec)
    {
        if (!_box->new_batch(BoxType::Constant::LINKBLOCK))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }
        if (!_box->send_out_data(data, count) ||
                !_box->end_batch())
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }
        ec = boost::system::error_code();
        return ec;
    }

    std::size_t send_and_recv(const boost::uint8_t *send,
                              std::size_t count,
                              boost::uint8_t *recv,
                              std::size_t maxLength,
                              boost::system::error_code &ec)
    {
        boost::uint32_t times = 3;
        while (times--)
        {
            _pro->send_frames(send, count, ec);
            if (ec)
            {
                continue;
            }
            return _pro->read_frames(recv, maxLength, ec);
        }
        ec = boost::asio::error::connection_refused;
        return 0;
    }

    boost::system::error_code start_keep_link(bool run, boost::system::error_code &ec)
    {
        if (!_box->keep_link(run))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }
        ec = boost::system::error_code();
        return ec;
    }

    boost::system::error_code set_timeout(boost::int64_t txB2B,
                                          boost::int64_t rxB2B,
                                          boost::int64_t txF2F,
                                          boost::int64_t rxF2F,
                                          boost::int64_t total,
                                          boost::system::error_code &ec)
    {
        ec = boost::system::error_code();
        return ec;
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