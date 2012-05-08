#ifndef __JM_DIAG_V1_MIKUNI_HPP__
#define __JM_DIAG_V1_MIKUNI_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/mikuni.h>
#include <jm/diag/v1/defaultfunc.h>

namespace JM
{
namespace Diag
{
namespace V1
{

template<typename BoxType>
class Mikuni : public Diag::Mikuni
{
public:

    Mikuni(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &s)
    : _box(box)
    , _shared(s)
    , _default()
    {
        _default.reset(new DefaultFunc<BoxType, Mikuni<BoxType> >(_box, _shared, this));
    }

    ~Mikuni()
    {
    }

    boost::system::error_code config(boost::system::error_code &ec)
    {
        boost::uint8_t valueOpen;
        boost::uint8_t valueClose;
        boost::uint8_t sendLine;
        boost::uint8_t recvLine;
        boost::uint8_t ctrlWord1;
        boost::uint8_t ctrlWord2;
        boost::uint8_t ctrlWord3;

        valueOpen = BoxType::Constant::PWC | BoxType::Constant::RZFC | BoxType::Constant::CK | BoxType::Constant::REFC;
        valueClose = BoxType::Constant::SET_NULL;
        sendLine = BoxType::Constant::SK_NO;
        recvLine = BoxType::Constant::RK1;
        ctrlWord1 = BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK | BoxType::Constant::SEL_SL | BoxType::Constant::UN_DB20;
        ctrlWord2 = 0xFF;
        ctrlWord3 = 2;

        if (!_box->set_comm_ctrl(valueOpen, valueClose) ||
                !_box->set_comm_line(sendLine, recvLine) ||
                !_box->set_comm_link(ctrlWord1, ctrlWord2, ctrlWord3) ||
                !_box->set_comm_baud(19200) ||
                !_box->set_comm_time(BoxType::Constant::SETBYTETIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(100))) ||
                !_box->set_comm_time(BoxType::Constant::SETWAITTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(1))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECBBOUT, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(400))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECFROUT, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500))) ||
                !_box->set_comm_time(BoxType::Constant::SETLINKTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500))))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        BoxType::sleep(typename BoxType::Seconds(1));
        ec = boost::system::error_code();
        return ec;
    }

    void finish_execute(bool isFinish)
    {
        if (isFinish)
        {
            _box->stop_now(true);
            _box->del_batch(_shared->buffID);
            _box->check_result(BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500)));
        }
    }

    std::size_t send_one_frame(const boost::uint8_t *data, std::size_t count,
                               boost::system::error_code &ec)
    {
        return send_one_frame(data, count, false, ec);
    }

    std::size_t send_frames(const boost::uint8_t *data, std::size_t count,
                            boost::system::error_code &ec)
    {
        return send_one_frame(data, count, ec);
    }

    std::size_t read_one_frame(boost::uint8_t *data, std::size_t maxLength,
                               boost::system::error_code &ec)
    {
        return read_one_frame(data, maxLength, true, ec);
    }

    std::size_t read_frames(boost::uint8_t *data, std::size_t maxLength,
                           boost::system::error_code &ec)
    {
        return read_one_frame(data, maxLength, ec);
    }

    boost::system::error_code set_keep_link(const boost::uint8_t *data, std::size_t count,
                                            boost::system::error_code &ec)
    {
        boost::array<boost::uint8_t, 255 > buff;
        std::size_t length;

        length = pack(data, count, buff.data(), buff.size(), ec);

        if (ec)
        {
            return ec;
        }

        return _default->set_keep_link(buff.data(), length, ec);
    }

    std::size_t send_and_recv(const boost::uint8_t *send,
                              std::size_t sendLength,
                              boost::uint8_t *recv,
                              std::size_t recvLength,
                              boost::system::error_code &ec)
    {
        return _default->send_and_recv(send, sendLength, recv, recvLength, ec);
    }

    boost::system::error_code set_timeout(boost::int64_t txB2B,
                                          boost::int64_t rxB2B,
                                          boost::int64_t txF2F,
                                          boost::int64_t rxF2F,
                                          boost::int64_t total,
                                          boost::system::error_code &ec)
    {
        return _default->set_timeout(txB2B, rxB2B, txF2F, rxF2F, total, ec);
    }

    boost::system::error_code start_keep_link(bool run, boost::system::error_code &ec)
    {
        return _default->start_keep_link(run, ec);
    }
private:

    std::size_t send_one_frame(const boost::uint8_t *data, std::size_t count,
                               bool needRecv, boost::system::error_code &ec)
    {
        return _default->send_one_frame(data, count, needRecv, ec);
    }

    std::size_t read_one_frame(boost::uint8_t *data, std::size_t maxLength,
                               bool isFinish, boost::system::error_code &ec)
    {
        std::size_t tempLength = 0;
        boost::array<boost::uint8_t, 255 > temp;
        boost::uint8_t before = 0;
        boost::uint8_t *p = temp.data();

        while (_box->read_bytes(p++, 1) == 1)
        {
            ++tempLength;
            if (before == 0x0D && (*p) == 0x0A)
            {
                break;
            }
            before = *p;
        }

        if (before == 0x0D && *p == 0x0A)
        {
            // break normal
            tempLength = unpack(temp.data(), tempLength, data, maxLength, ec);
        }
        else
        {
            ec = boost::system::error_code(boost::system::errc::bad_message,
                                           boost::system::get_system_category());
            tempLength = 0;
        }
        finish_execute(isFinish);
        return tempLength;
    }


private:
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<DefaultFunc<BoxType, Mikuni<BoxType> > > _default;
};
}
}
}

#endif
