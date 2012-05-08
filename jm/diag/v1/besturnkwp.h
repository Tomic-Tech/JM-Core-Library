#ifndef __JM_DIAG_V1_BESTURN_KWP_HPP__
#define __JM_DIAG_V1_BESTURN_KWP_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/utils.h>
#include <jm/diag/kwp2000.h>
#include <jm/diag/v1/defaultfunc.h>

namespace JM
{
namespace Diag
{
namespace V1
{

template<typename BoxType>
class BesturnKWP : public KWP2000
{
public:

    BesturnKWP(const boost::shared_ptr<BoxType> &box,
                              const boost::shared_ptr<Shared> &s)
    : _box(box)
    , _shared(s)
    , _sendLine(0)
    , _recvLine(0)
    {
        _default.reset(new DefaultFunc<BoxType, BesturnKWP<BoxType> >(_box,
                       _shared, this));
    }

    std::size_t pack(const boost::uint8_t* src,
                                   std::size_t srcLength,
                                   boost::uint8_t *tar,
                                   std::size_t tarLength,
                                   boost::system::error_code &ec)
    {
        if (srcLength == 0)
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        tar[0] = 0x80;
        tar[1] = _opts.targetAddress;
        tar[2] = _opts.sourceAddress;
        tar[3] = high_byte(low_word(srcLength));
        tar[4] = low_byte(low_word(srcLength));

        memcpy(tar + 5, src, srcLength);
        boost::uint16_t checksum = 0;
        for (std::size_t i = 0; i < srcLength; ++i)
        {
            checksum += tar[i];
        }
        tar[srcLength + 5] = high_byte(checksum);
        tar[srcLength + 6] = low_byte(checksum);
        ec = boost::system::error_code();
        return srcLength + 7;
    }

    std::size_t unpack(const boost::uint8_t *src,
                                     std::size_t srcLength,
                                     boost::uint8_t *tar,
                                     std::size_t tarLength,
                                     boost::system::error_code &ec)
    {
        if (srcLength == 0)
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        std::size_t length = (src[5] << 8) + src[6];
        if (length != (srcLength - 7))
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        memcpy(tar, src + 5, length);
        ec = boost::system::error_code();
        return length;
    }

    boost::system::error_code
    fast_init(const boost::uint8_t *data,
              std::size_t count, boost::system::error_code &ec)
    {
        boost::uint8_t valueOpen = 0;
        boost::array<boost::uint8_t, 255 > packEnter;
        std::size_t length;

        if (_opts.lLine)
        {
            valueOpen = BoxType::Constant::PWC | BoxType::Constant::RZFC | BoxType::Constant::CK;
        }
        else
        {
            valueOpen = BoxType::Constant::PWC | BoxType::Constant::RZFC | BoxType::Constant::CK;
        }

        if (!_box->set_comm_ctrl(valueOpen, BoxType::Constant::SET_NULL) ||
                !_box->set_comm_line(_sendLine, _recvLine) ||
                !_box->set_comm_link(BoxType::Constant::RS_232 |
                                     BoxType::Constant::BIT9_MARK |
                                     BoxType::Constant::SEL_SL |
                                     BoxType::Constant::UN_DB20,
                                     BoxType::Constant::SET_NULL,
                                     BoxType::Constant::SET_NULL) ||
                !_box->set_comm_baud(_opts.baud) ||
                !_box->set_comm_time(BoxType::Constant::SETBYTETIME,
                                     BoxType::to_micro_seconds(typename BoxType::MilliSeconds(5))) ||
                !_box->set_comm_time(BoxType::Constant::SETWAITTIME,
                                     BoxType::to_micro_seconds(typename BoxType::MilliSeconds(0))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECBBOUT,
                                     BoxType::to_micro_seconds(typename BoxType::MilliSeconds(400))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECFROUT,
                                     BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500))) ||
                !_box->set_comm_time(BoxType::Constant::SETLINKTIME,
                                     BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500))))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        BoxType::sleep(typename BoxType::Seconds(1));

        _shared->buffID = 0;

        if (!_box->new_batch(_shared->buffID))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        length = pack(data, count, packEnter.data(), packEnter.size(), ec);

        if (!_box->set_line_level(BoxType::Constant::COMS, BoxType::Constant::SET_NULL) ||
                !_box->commbox_delay(BoxType::to_micro_seconds(typename BoxType::MilliSeconds(25))) ||
                !_box->set_line_level(BoxType::Constant::SET_NULL, BoxType::Constant::COMS) ||
                !_box->commbox_delay(BoxType::to_micro_seconds(typename BoxType::MilliSeconds(25))) ||
                !_box->send_out_data(packEnter.data(), length) ||
                !_box->run_receive(BoxType::Constant::REC_FR) ||
                !_box->end_batch())
        {
            _box->del_batch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        if (!_box->run_batch(&(_shared->buffID), 1, false))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        length = read_one_frame(packEnter.data(), packEnter.size(), ec);
        if (ec)
        {
            return ec;
        }

        _box->set_comm_time(BoxType::Constant::SETWAITTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(55)));
        ec = boost::system::error_code();
        return ec;
    }

    boost::system::error_code addr_init(boost::uint8_t addrCode,
                                                      boost::system::error_code &ec)
    {
        ec = boost::asio::error::operation_not_supported;
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

    boost::system::error_code config(const KWP2000::Options &opts, boost::system::error_code &ec)
    {
        ec = KWP2000::config(opts, ec);
        if (ec)
        {
            return ec;
        }

        if (_shared->cnn == CN_OBDII_16)
        {
            switch (_opts.comLine)
            {
            case 7:
                _sendLine = BoxType::Constant::SK1;
                _recvLine = BoxType::Constant::RK1;
                break;
            default:
                ec = boost::asio::error::invalid_argument;
                break;
            }

        }
        else
        {
            ec = boost::asio::error::invalid_argument;
        }

        return ec;
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
        StaticBytes buff;
        std::size_t length;
        length = pack(data, count, buff.data(), buff.size(), ec);
        return _default->set_keep_link(buff.data(), length, ec);
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

    std::size_t read_one_frame(boost::uint8_t *data, std::size_t maxLength,
                                             bool isFinish, boost::system::error_code &ec)
    {
        static boost::array<boost::uint8_t, 3 > temp;
        static boost::array<boost::uint8_t, 255 > result;
        std::size_t frameLength = 0;
        std::size_t length;
        boost::uint8_t checksum;
        std::size_t i;

        length = _box->read_bytes(temp.data(), temp.size());

        if (length <= 0)
        {
            finish_execute(isFinish);
            ec = boost::asio::error::timed_out;
            return 0;
        }

        if (temp[1] == _opts.sourceAddress)
        {
            if (temp[0] == 0x80)
            {
                boost::uint8_t b;
                length = _box->read_bytes(&b, 1);
                if (length <= 0)
                {
                    finish_execute(isFinish);
                    ec = boost::asio::error::timed_out;
                    return 0;
                }
                length = b;
                if (length <= 0)
                {
                    finish_execute(isFinish);
                    ec = boost::asio::error::message_size;
                    return 0;
                }
                memcpy(result.data(), temp.data(), temp.size());
                frameLength += temp.size();
                memcpy(result.data() + frameLength, &b, 1);
                ++frameLength;
                length = _box->read_bytes(result.data() + KWP80_HEADER_LENGTH, length + KWP_CHECKSUM_LENGTH);
                frameLength += length;
            }
            else
            {
                length = temp[0] - 0x80;
                if (length <= 0)
                {
                    return 0;
                }

                memcpy(result.data(), temp.data(), temp.size());
                frameLength += temp.size();
                length = _box->read_bytes(result.data() + temp.size(), length + KWP_CHECKSUM_LENGTH);
                frameLength += length;
            }
        }
        else
        {
            if (temp[0] == 0x00)
            {
                length = temp[1];
                if (length <= 0)
                {
                    finish_execute(isFinish);
                    ec = boost::asio::error::message_size;
                    return 0;
                }
                memcpy(result.data(), temp.data(), temp.size());
                frameLength += temp.size();
                length = _box->read_bytes(result.data() + temp.size(), length);
                frameLength += length;
            }
            else
            {
                length = temp[0];
                if (length <= 0)
                {
                    finish_execute(isFinish);
                    ec = boost::asio::error::message_size;
                    return 0;
                }
                memcpy(result.data(), temp.data(), temp.size());
                frameLength += temp.size();
                length = _box->read_bytes(result.data() + temp.size(), length - KWP_CHECKSUM_LENGTH);
                frameLength += length;
            }
        }

        finish_execute(isFinish);
        if (frameLength <= 0)
        {
            return frameLength;
        }

        checksum = 0;
        for (i = 0; i < frameLength - 1; ++i)
        {
            checksum += result[i];
        }

        if (checksum != result[frameLength - 1])
        {
            ec = boost::system::error_code(boost::system::errc::bad_message,
                                           boost::system::get_system_category());
            return 0;
        }

        return unpack(result.data(), frameLength, data, maxLength, ec);
    }

    boost::int32_t send_one_frame(const boost::uint8_t *data,
                                                std::size_t count, bool isFinish, boost::system::error_code &ec)
    {
        return _default->send_one_frame(data, count, isFinish, ec);
    }

    std::size_t send_and_recv(const boost::uint8_t *send,
                                            std::size_t sendLength,
                                            boost::uint8_t *recv,
                                            std::size_t recvLength,
                                            boost::system::error_code &ec)
    {
        return _default->send_and_recv(send, sendLength, recv, recvLength, ec);
    }
private:
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<DefaultFunc<BoxType, BesturnKWP<BoxType> > >_default;
    boost::uint8_t _sendLine;
    boost::uint8_t _recvLine;
};
}
}
}

#endif
