#ifndef __DIAG_V1_kwp1281_HPP__
#define __DIAG_V1_kwp1281_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/kwp1281.h>
#include <jm/diag/v1/defaultfunc.h>
#include <jm/utils.h>

namespace JM
{
namespace Diag
{
namespace V1
{

template<typename BoxType>
class KWP1281 : public Diag::KWP1281
{
public:

    KWP1281(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &s)
    : _box(box)
    , _shared(s)
    , _default()
    {
        _default.reset(new DefaultFunc<BoxType, KWP1281<BoxType> >(_box, _shared, this));
    }

    ~KWP1281()
    {
    }

    void finish_execute(bool isFinish)
    {
        if (isFinish)
        {
            _box->del_batch(_shared->buffID);
            _box->check_result(BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500)));
        }
    }

    boost::system::error_code addr_init(boost::uint8_t addrCode, boost::system::error_code &ec)
    {
        bool dl0;
        boost::array<boost::uint8_t, 255 > temp;
        std::size_t length;
        boost::uint8_t valueOpen;
        boost::uint8_t valueClose;
        boost::uint8_t ctrlWord1;
        boost::uint8_t ctrlWord2;
        boost::uint8_t ctrlWord3;

        _shared->buffID = 0;
        _box->stop_now(true);
        _box->check_result(BoxType::to_micro_seconds(typename BoxType::MilliSeconds(50)));

        dl0 = (_lLine == BoxType::Constant::SK_NO && _kLine == BoxType::Constant::RK_NO) ? true : false;
        valueOpen = BoxType::Constant::PWC | BoxType::Constant::REFC | BoxType::Constant::RZFC | (dl0 ? BoxType::Constant::DLC0 : BoxType::Constant::CK);
        valueClose = BoxType::Constant::SET_NULL;
        ctrlWord1 = BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK | (dl0 ? BoxType::Constant::SEL_DL0 : BoxType::Constant::SEL_SL) | BoxType::Constant::SET_DB20;
        ctrlWord2 = 0xFF;
        ctrlWord3 = BoxType::Constant::INVERTBYTE | 1;

        if (!_box->set_comm_ctrl(valueOpen, valueClose) ||
                !_box->set_comm_line(_lLine, _kLine) ||
                !_box->set_comm_link(ctrlWord1, ctrlWord2, ctrlWord3) ||
                !_box->set_comm_baud(5) ||
                !_box->set_comm_time(BoxType::Constant::SETBYTETIME, 0) ||
                !_box->set_comm_time(BoxType::Constant::SETWAITTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(25))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECBBOUT, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(610))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECFROUT, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(610))) ||
                !_box->set_comm_time(BoxType::Constant::SETLINKTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(710))))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        BoxType::sleep(typename BoxType::Seconds(1));

        if (!_box->new_batch(_shared->buffID))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        if (!_box->send_out_data(&addrCode, 1) ||
                !_box->set_comm_line(_kLine == BoxType::Constant::RK1 ? BoxType::Constant::SK_NO : _lLine, _kLine) ||
                !_box->run_receive(BoxType::Constant::SET55_BAUD) ||
                !_box->run_receive(BoxType::Constant::REC_LEN_1) ||
                !_box->turn_over_one_by_one() ||
                !_box->run_receive(BoxType::Constant::REC_LEN_1) ||
                !_box->set_comm_time(BoxType::Constant::SETBYTETIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(2))) ||
                !_box->set_comm_time(BoxType::Constant::SETWAITTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(2))) ||
                !_box->run_receive(BoxType::Constant::REC_FR) ||
                !_box->turn_over_one_by_one() ||
                !_box->run_receive(BoxType::Constant::REC_LEN_1) ||
                !_box->end_batch())
        {
            _box->del_batch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return ec;
        }
        if (!_box->run_batch(&_shared->buffID, 1, false))
        {
            _box->del_batch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        if (_box->read_data(temp.data(), 2, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(3500))) != 2)
        {
            _box->del_batch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        length = read_one_frame(temp.data(), temp.size(), ec);
        if (ec)
        {
            return ec;
        }

        if (!_box->check_result(BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500))))
        {
            _box->del_batch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        if (!_box->del_batch(_shared->buffID))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        _frameCounter = temp[0];
        ec = boost::system::error_code();
        return ec;
    }

    boost::system::error_code config(const Options &opts, boost::system::error_code &ec)
    {
        ec = Diag::KWP1281::config(opts, ec);
        if (ec)
        {
            return ec;
        }

        if (_shared->cnn == CN_AUDI_4 ||
                _shared->cnn == CN_OBDII_16)
        {
            if (_opts.comLine != 0)
            {
                _kLine = BoxType::Constant::RK1;
            }
            else
            {
                _kLine = BoxType::Constant::RK_NO;
            }
            if (_opts.lLine)
            {
                _lLine = BoxType::Constant::SK2;
            }
            else
            {
                _lLine = BoxType::Constant::SK_NO;
            }
        }
        else
        {
            ec = boost::asio::error::invalid_argument;
            return ec;
        }
        ec = boost::system::error_code();
        return ec;

    }

    std::size_t send_one_frame(const boost::uint8_t *data, std::size_t count,
                               bool needRecv, boost::system::error_code &ec)
    {
        boost::array<boost::uint8_t, 255 > sendBuff;
        std::size_t length;

        _shared->buffID = 0;

        if (!_box->new_batch(_shared->buffID))
        {
            ec = boost::asio::error::connection_refused;
            return 0;
        }

        length = pack(data, count, sendBuff.data(), sendBuff.size(), ec);

        if (ec)
        {
            return 0;
        }

        if (needRecv)
        {
            if (!_box->turn_over_one_by_one() ||
                    !_box->send_out_data(sendBuff.data(), length) ||
                    !_box->update_buff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->turn_over_one_by_one() ||
                    !_box->run_receive(BoxType::Constant::REC_FR) ||
                    !_box->turn_over_one_by_one() ||
                    !_box->run_receive(BoxType::Constant::REC_LEN_1) ||
                    !_box->update_buff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->turn_over_one_by_one() ||
                    !_box->run_receive(BoxType::Constant::REC_FR) ||
                    !_box->turn_over_one_by_one() ||
                    !_box->run_receive(BoxType::Constant::REC_LEN_1) ||
                    !_box->end_batch() ||
                    !_box->run_batch(&_shared->buffID, 1, false))
            {
                _box->del_batch(_shared->buffID);
                ec = boost::asio::error::connection_refused;
                return 0;
            }
            boost::this_thread::sleep(boost::posix_time::microseconds((length * _shared->reqByteToByte + _shared->reqWaitTime) / 1000));

        }
        else
        {
            if (!_box->turn_over_one_by_one() ||
                    !_box->send_out_data(sendBuff.data(), length) ||
                    !_box->update_buff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->turn_over_one_by_one() ||
                    !_box->run_receive(BoxType::Constant::REC_FR) ||
                    !_box->turn_over_one_by_one() ||
                    !_box->run_receive(BoxType::Constant::REC_LEN_1) ||
                    !_box->update_buff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->end_batch() ||
                    !_box->run_batch(&_shared->buffID, 1, false))
            {
                _box->del_batch(_shared->buffID);
                ec = boost::asio::error::connection_refused;
                return 0;
            }
        }
        if (_box->read_bytes(sendBuff.data(), length - 1) != (length - 1))
        {
            ec = boost::asio::error::timed_out;
            return 0;
        }
        return count;
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
                               bool isFinish, boost::system::error_code &ec)
    {
        std::size_t pos = 0;
        std::size_t len;
        boost::array<boost::uint8_t, 255 > temp;

        if (_box->read_bytes(temp.data(), 1) != 1)
        {
            ec = boost::asio::error::timed_out;
            return 0;
        }

        ++pos;
        len = temp[0];

        if (_box->read_bytes(temp.data() + pos, len) != len)
        {
            ec = boost::asio::error::timed_out;
            return 0;
        }

        pos += len;
        finish_execute(isFinish);

        return unpack(temp.data(), pos, data, maxLength, ec);
    }

    std::size_t read_one_frame(boost::uint8_t *data,
                               std::size_t maxLength, boost::system::error_code &ec)
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
        boost::uint8_t add1 = 0;
        boost::uint8_t add2 = 0;
        boost::array<boost::uint8_t, 3 > tempBuff;

        _buffIDAddr[0] = BoxType::Constant::LINKBLOCK;
        if (_box->new_batch(BoxType::Constant::LINKBLOCK))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        if (!_box->turn_over_one_by_one() ||
                !_box->send_out_data(data, data[0]))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        _buffIDAddr[1] = data[0] - 2;

        if (!_box->turn_over_one_by_one() ||
                !_box->send_out_data(data, 1) ||
                !((add1 = _box->update_buff(BoxType::Constant::INC_DATA, _buffIDAddr.data())) == 0) ||
                !_box->turn_over_one_by_one() ||
                !_box->run_receive(BoxType::Constant::REC_FR) ||
                !_box->turn_over_one_by_one() ||
                !_box->run_receive(BoxType::Constant::REC_LEN_1) ||
                !((add2 = _box->update_buff(BoxType::Constant::INC_DATA, _buffIDAddr.data())) == 0) ||
                !_box->end_batch())
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        tempBuff[0] = BoxType::Constant::LINKBLOCK;
        tempBuff[1] = add1;
        tempBuff[2] = _buffIDAddr[1];

        if (!_box->get_abs_add(BoxType::Constant::LINKBLOCK, tempBuff[2]))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        if (!_box->update_buff(BoxType::Constant::UPDATE_1BYTE, tempBuff.data()))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        tempBuff[1] = add2;

        if (!_box->update_buff(BoxType::Constant::UPDATE_1BYTE, tempBuff.data()))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        ec = boost::system::error_code();
        return ec;
    }

    std::size_t send_and_recv(const boost::uint8_t *send,
                              std::size_t sendLength,
                              boost::uint8_t *recv,
                              std::size_t recvLength,
                              boost::system::error_code &ec)
    {
        std::size_t length = send_one_frame(send, sendLength, ec);
        if (ec)
        {
            return 0;
        }
        return read_one_frame(recv, recvLength, ec);
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
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<DefaultFunc<BoxType, KWP1281<BoxType> > >_default;
    boost::array<boost::uint8_t, 2 > _buffIDAddr; // 0 == buffID, 1 == buffAddr
    boost::uint8_t _kLine;
    boost::uint8_t _lLine;
};
}
}
}

#endif
