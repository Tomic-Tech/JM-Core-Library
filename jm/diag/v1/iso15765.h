#ifndef __JM_DIAG_V1_iso15765_HPP__
#define __JM_DIAG_V1_iso15765_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/canbus.h>
#include <jm/diag/v1/defaultfunc.h>
#include <jm/utils.h>

namespace JM
{
namespace Diag
{
namespace V1
{
static const boost::uint8_t SJA_BTR_CODETABLE[30] = {
    0xBF, 0xFF, // 5KBPS
    0xEF, 0xFF, // 10KBPS
    0xD7, 0xFF, // 20KBPS
    0xCB, 0xFF, // 40KBPS
    0x47, 0x2F, // 50KBPS
    0xC5, 0xFF, // 80KBPS
    0xC9, 0xA7, // 100KBPS
    0x45, 0x2B, // 125KBPS
    0x04, 0xA7, // 200KBPS
    0x01, 0x1C, // 250KBPS
    0x02, 0x25, // 400KBPS
    0x01, 0x45, // 500KBPS
    0x80, 0xB6, // 666KBPS
    0x00, 0x16, // 800KBPS
    0x01, 0x12 // 1000KBPS
};

template<typename BoxType>
class ISO15765 : public Canbus
{
public:
    static const boost::uint8_t SET_CANBEGIN = 0xF0;
    static const boost::uint8_t SET_CANBAUD = 0xF1;
    static const boost::uint8_t SET_CANACR = 0xF2;
    static const boost::uint8_t SET_CANAMR = 0xF3;
    static const boost::uint8_t SET_CANMODE = 0xF4;
    static const boost::uint8_t SET_CANCHAN = 0xF5;
    static const boost::uint8_t SET_CANPRIOR = 0xF6;
    static const boost::uint8_t SET_CANEND = 0xFA;
    static const boost::uint8_t SET_CANRESET = 0xFB;

    static const boost::uint8_t CAN_ACR_ACF1 = 0x20;
    static const boost::uint8_t CAN_ACR_ACF2 = 0x28;
    static const boost::uint8_t CAN_ACR_ACF3 = 0x30;
    static const boost::uint8_t CAN_ACR_ACF4 = 0x38;

    static const boost::uint8_t CAN_AMR_ACF1 = 0x24;
    static const boost::uint8_t CAN_AMR_ACF2 = 0x2C;
    static const boost::uint8_t CAN_AMR_ACF3 = 0x34;
    static const boost::uint8_t CAN_AMR_ACF4 = 0x3C;

    static const boost::uint8_t ACF1_FT1_PRIO = 0x01;
    static const boost::uint8_t ACF1_FT2_PRIO = 0x02;
    static const boost::uint8_t ACF2_FT1_PRIO = 0x04;
    static const boost::uint8_t ACF2_FT2_PRIO = 0x08;
    static const boost::uint8_t ACF3_FT1_PRIO = 0x10;
    static const boost::uint8_t ACF3_FT2_PRIO = 0x20;
    static const boost::uint8_t ACF4_FT1_PRIO = 0x40;
    static const boost::uint8_t ACF4_FT2_PRIO = 0x80;

    static const boost::uint8_t ACF1_FT1_CHAN = 0x01;
    static const boost::uint8_t ACF1_FT2_CHAN = 0x02;
    static const boost::uint8_t ACF2_FT1_CHAN = 0x04;
    static const boost::uint8_t ACF2_FT2_CHAN = 0x08;
    static const boost::uint8_t ACF3_FT1_CHAN = 0x10;
    static const boost::uint8_t ACF3_FT2_CHAN = 0x20;
    static const boost::uint8_t ACF4_FT1_CHAN = 0x40;
    static const boost::uint8_t ACF4_FT2_CHAN = 0x80;

    static const boost::uint8_t STD_FRAMEID_LENGTH = 0x02;
    static const boost::uint8_t EXT_FRAMEID_LENGTH = 0x04;
    static const boost::uint8_t SJA_OK = 0;
public:

    ISO15765(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &s)
    : _box(box)
    , _shared(s)
    , _default()
    {
        _default.reset(new DefaultFunc<BoxType, ISO15765<BoxType> >(_box, _shared, this));
    }

    void finish_execute(bool isFinish)
    {
        if (isFinish)
        {
            _box->stop_now(false);
            _box->del_batch(_shared->buffID);
            _box->check_result(BoxType::to_micro_seconds(typename BoxType::MilliSeconds(200)));
        }
    }

    inline bool reset()
    {
        return do_cmd(SET_CANRESET, 0);
    }

    boost::system::error_code config(const Canbus::Options &opts, boost::system::error_code &ec)
    {
        ec = Canbus::config(opts, ec);
        if (ec)
        {
            return ec;
        }

        _opts.targetID = (_opts.targetID << 5) & 0x0000FFFF;

        boost::uint32_t localID = 0;
        boost::uint8_t ctrlWord1;

        ctrlWord1 = BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK | BoxType::Constant::SEL_SL | BoxType::Constant::UN_DB20;

        if (!_box->set_comm_ctrl(BoxType::Constant::RZFC, BoxType::Constant::SET_NULL) ||
                !_box->set_comm_line(BoxType::Constant::SK5, BoxType::Constant::SK7) ||
                !_box->set_comm_link(ctrlWord1, BoxType::Constant::SET_NULL, BoxType::Constant::SET_NULL) ||
                !_box->set_comm_baud(57600) ||
                !_box->set_comm_time(BoxType::Constant::SETBYTETIME, 500) ||
                !_box->set_comm_time(BoxType::Constant::SETWAITTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(55))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECBBOUT, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(100))) ||
                !_box->set_comm_time(BoxType::Constant::SETRECFROUT, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(200))) ||
                !_box->set_comm_time(BoxType::Constant::SETLINKTIME, BoxType::to_micro_seconds(typename BoxType::MilliSeconds(500))))
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }
        localID = _opts.idVector->at(0);

        BoxType::sleep(typename BoxType::MilliSeconds(100));

        if (!begin_set() ||
                !set_baud(B500K) ||
                !set_acr(CAN_ACR_ACF1,
                         high_byte(high_word(localID << 21)),
                         low_byte(high_word(localID << 21)),
                         high_byte(low_word(localID << 21)),
                         low_byte(low_word(localID << 21))) ||
                !set_amr(CAN_AMR_ACF1, 0x00, 0x1F, 0xFF, 0xFF) ||
                !set_mode(0x55) ||
                !set_prior(0xFF) ||
                !set_chan(ACF1_FT1_CHAN) ||
                !end_set())
        {
            ec = boost::asio::error::connection_refused;
            return ec;
        }

        static boost::uint8_t keepLink[2] = {0x3E, 0x01};
        ec = set_keep_link(keepLink, 2, ec);
        ec = boost::system::error_code();
        return ec;
    }

    std::size_t send_one_frame(const boost::uint8_t *data, std::size_t count,
                               bool isFinish, boost::system::error_code &ec)
    {
        return _default->send_one_frame(data, count, isFinish, ec);
    }

    std::size_t send_one_frame(const boost::uint8_t *data, std::size_t count,
                               boost::system::error_code &ec)
    {
        return send_one_frame(data, count, false, ec);
    }

    std::size_t send_frames(const boost::uint8_t *data, std::size_t count,
                            boost::system::error_code &ec)
    {
        if (count <= 0 || count > 0x0FFF)
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        if (count < 8)
        {
            boost::array<boost::uint8_t, 8 > temp;
            memset(temp.data(), 0, temp.size());

            temp[0] = low_byte(count);
            memcpy(temp.data() + 1, data, count);
            return send_one_frame(temp.data(), temp.size(), true, ec);
        }
        else
        {
            std::size_t frame_count = (count + 1) / 7;
            std::size_t last_data = (count + 1) % 7;
            std::size_t pos = 0;
            boost::uint8_t seq = 0x21;
            std::size_t frame_index = 0;

            if (last_data != 0)
                ++frame_count;

            for (; frame_index < frame_count; ++frame_index)
            {
                if (0 == frame_index)
                {
                    boost::array<boost::uint8_t, 8 > temp;
                    temp[0] = (0x10 | high_byte(low_word(count)));
                    temp[1] = low_byte(low_word(count));
                    memcpy(temp.data() + 2, data + pos, 6);
                    pos += 6;
                    send_one_frame(temp.data(), temp.size(), true, ec);
                    if (ec)
                    {
                        return 0;
                    }
                    read_one_frame(temp.data(), temp.size(), ec);
                    if (ec)
                    {
                        return 0;
                    }
                }
                else if (frame_index == (frame_count - 1))
                {
                    boost::array<boost::uint8_t, 8 > temp;
                    memset(temp.data(), 0, temp.size());
                    temp[0] = seq;

                    memcpy(temp.data() + 1, data + pos, count - pos);
                    if (send_one_frame(temp.data(), temp.size(), true, ec) != (count - pos + 1))
                        return 0;
                    ec = boost::system::error_code();
                    return count;
                }
                else
                {
                    boost::array<boost::uint8_t, 8 > temp;
                    temp[0] = seq;
                    memcpy(temp.data() + 1, data + pos, 7);
                    pos += 7;
                    if (send_one_frame(temp.data(), temp.size(), false, ec) != 8)
                        return 0;

                    if (seq == 0x2F)
                        seq = 0x20;
                    else
                        ++seq;
                }
            }
        }
        ec = boost::asio::error::timed_out;
        return 0;
    }

    std::size_t read_one_frame(boost::uint8_t *data, std::size_t maxLength,
                               boost::system::error_code &ec)
    {
        return read_one_frame(data, maxLength, true, ec);
    }

    std::size_t read_frames(boost::uint8_t *data, std::size_t maxLength,
                            boost::system::error_code &ec)
    {
        boost::array<boost::uint8_t, 8 > firstFrame;
        std::size_t length;

        length = read_one_frame(firstFrame.data(), firstFrame.size(), false, ec);

        if (ec)
        {
            return 0;
        }

        // Check if the flow control
        if (firstFrame[0] == 0x30)
        {
            finish_execute(true);
            memcpy(data, firstFrame.data(), length);
            return length;
        }

        if ((firstFrame[0] & 0x10) == 0x10)
        {
            // Multi-Frame
            static std::size_t userDataCount;
            static std::size_t resetFrameCount;
            static std::size_t dataLength;
            static std::size_t restDataCount;
            static std::size_t i;
            static std::size_t pos;
            static boost::array<boost::uint8_t, 0xFFF > restFrames;
            static boost::array<boost::uint8_t, 8 > unpackedBuff;

            finish_execute(true);

            if (send_one_frame(_flowControl.data(), _flowControl.size(), true, ec) != 8)
            {
                return 0;
            }

            pos = 0;
            userDataCount = ((firstFrame[0] & 0x0F) << 8) | (firstFrame[1] & 0xFF);
            userDataCount -= 6;
            resetFrameCount = (userDataCount / 7) + ((userDataCount % 7) ? 1 : 0);
            memcpy(data + pos, firstFrame.data() + 2, length - 2);
            pos += length - 2;

            if (_opts.idMode == StdID)
            {
                dataLength = STD_FRAMEID_LENGTH + 8;
            }
            else
            {
                dataLength = EXT_FRAMEID_LENGTH + 8;
            }

            restDataCount = dataLength * resetFrameCount;

            if (_box->read_bytes(restFrames.data(), restDataCount) != restDataCount)
            {
                finish_execute(true);
                ec = boost::asio::error::timed_out;
                return 0;
            }

            finish_execute(true);
            for (i = 0; i < resetFrameCount; ++i)
            {
                // In iso15765, we always read 8 bytes data.
                // When in multiple frames, the first byte is frame index.
                // And the rest 7 bytes is the data, the tail will be 0x00
                // if bytes reach the userDataCount.
                length = unpack(restFrames.data() + (i * dataLength),
                                dataLength,
                                unpackedBuff.data(),
                                unpackedBuff.size(),
                                ec);
                if (userDataCount > length)
                {
                    memcpy(data + pos, unpackedBuff.data() + 1, length); // +1 to skip the frame index.
                    userDataCount -= length;
                    pos += length;
                }
                else
                {
                    memcpy(data + pos, unpackedBuff.data(), userDataCount); // +1 to skip the frame index.
                    pos += userDataCount;
                }
            }
        }
        else
        {
            // Pending
            while (firstFrame[1] == 0x7F && firstFrame[3] == 0x78)
            {
                length = read_one_frame(firstFrame.data(), firstFrame.size(), false, ec);
            }
            finish_execute(true);
            memcpy(data, firstFrame.data() + 1, length - 1);
            return length;
        }
        ec = boost::asio::error::timed_out;
        return 0;
    }

    boost::system::error_code set_keep_link(const boost::uint8_t *data, std::size_t count,
                                            boost::system::error_code &ec)
    {
        boost::array<boost::uint8_t, 255 > buff;
        std::size_t length;

        length = pack(data, count, buff.data(), buff.size(), ec);

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

    std::size_t send_cmd(const boost::uint8_t *data, std::size_t count)
    {
        _shared->buffID = 0;
        if (!_box->new_batch(_shared->buffID))
        {
            return 0;
        }

        if (!_box->send_out_data(data, count) ||
                !_box->run_receive(BoxType::Constant::RECEIVE) ||
                !_box->end_batch() ||
                !_box->run_batch(&_shared->buffID, 1, false))
        {
            _box->del_batch(_shared->buffID);
            return 0;
        }

        BoxType::sleep(typename BoxType::MicroSeconds(count * _shared->reqByteToByte + _shared->reqWaitTime));
        return count;
    }

    std::size_t read_cmd(boost::uint8_t *buff, std::size_t maxLength)
    {
        std::size_t length;

        if (_box->read_bytes(buff, 3) != 3)
        {
            finish_execute(true);
            return 0;
        }

        length = buff[0] & 0x0F;

        switch (buff[0] & 0xC0)
        {
        case StdID | DataFrame:
        case StdID | RemoteFrame:
            length += STD_FRAMEID_LENGTH;
            break;
        case (ExtID | DataFrame):
        case (ExtID | RemoteFrame):
            length += EXT_FRAMEID_LENGTH;
            break;
        default:
            finish_execute(true);
            return 0;
        }

        length -= 2;
        if (length > 0)
        {
            if (_box->read_bytes(buff + 3, length) != length)
            {
                finish_execute(true);
                return 0;
            }
        }
        finish_execute(true);
        return length + 3;
    }

    bool do_cmd(boost::uint8_t cmd, boost::uint8_t inf)
    {
        boost::array<boost::uint8_t, 255 > buff;
        buff[0] = 0x20 | 2;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = cmd;
        buff[4] = inf;

        std::size_t length;

        if (send_cmd(buff.data(), 5) != 5)
        {
            return false;
        }
        length = read_cmd(buff.data(), buff.size());

        if (length <= 0)
        {
            return false;
        }

        if (buff[4] != SJA_OK)
        {
            return false;
        }

        return true;
    }

    inline bool begin_set()
    {
        return do_cmd(SET_CANBEGIN, 0);
    }

    inline bool set_mode(boost::uint8_t mode)
    {
        return do_cmd(SET_CANMODE, mode);
    }

    inline bool set_prior(boost::uint8_t prior)
    {
        return do_cmd(SET_CANPRIOR, prior);
    }

    inline bool set_chan(boost::uint8_t chan)
    {
        return do_cmd(SET_CANCHAN, chan);
    }

    inline bool end_set()
    {
        return do_cmd(SET_CANEND, 0);
    }

    bool set_baud(Baudrate baud)
    {
        boost::array<boost::uint8_t, 255 > buff;
        buff[0] = 0x20 | 3;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = SET_CANBAUD;

        switch (baud)
        {
        case B1000K:
            buff[4] = SJA_BTR_CODETABLE[28];
            buff[5] = SJA_BTR_CODETABLE[29];
            break;
        case B800K:
            buff[4] = SJA_BTR_CODETABLE[26];
            buff[5] = SJA_BTR_CODETABLE[27];
            break;
        case B666K:
            buff[4] = SJA_BTR_CODETABLE[24];
            buff[5] = SJA_BTR_CODETABLE[25];
            break;
        case B500K:
            buff[4] = SJA_BTR_CODETABLE[22];
            buff[5] = SJA_BTR_CODETABLE[23];
            break;
        case B400K:
            buff[4] = SJA_BTR_CODETABLE[20];
            buff[5] = SJA_BTR_CODETABLE[21];
            break;
        case B250K:
            buff[4] = SJA_BTR_CODETABLE[18];
            buff[5] = SJA_BTR_CODETABLE[19];
            break;
        case B200K:
            buff[4] = SJA_BTR_CODETABLE[16];
            buff[5] = SJA_BTR_CODETABLE[17];
            break;
        case B125K:
            buff[4] = SJA_BTR_CODETABLE[14];
            buff[5] = SJA_BTR_CODETABLE[15];
            break;
        case B100K:
            buff[4] = SJA_BTR_CODETABLE[12];
            buff[5] = SJA_BTR_CODETABLE[13];
            break;
        case B80K:
            buff[4] = SJA_BTR_CODETABLE[10];
            buff[5] = SJA_BTR_CODETABLE[11];
            break;
        case B50K:
            buff[4] = SJA_BTR_CODETABLE[8];
            buff[5] = SJA_BTR_CODETABLE[9];
            break;
        case B40K:
            buff[4] = SJA_BTR_CODETABLE[6];
            buff[5] = SJA_BTR_CODETABLE[7];
            break;
        case B20K:
            buff[4] = SJA_BTR_CODETABLE[4];
            buff[5] = SJA_BTR_CODETABLE[5];
            break;
        case B10K:
            buff[4] = SJA_BTR_CODETABLE[2];
            buff[5] = SJA_BTR_CODETABLE[3];
            break;
        case B5K:
            buff[4] = SJA_BTR_CODETABLE[0];
            buff[5] = SJA_BTR_CODETABLE[1];
            break;
        default:
            return false;
            break;
        }

        if (send_cmd(buff.data(), 6) != 6)
        {
            return false;
        }

        if (read_cmd(buff.data(), buff.size()) <= 0)
        {
            return false;
        }

        if (buff[4] != SJA_OK)
        {
            return false;
        }

        return true;
    }

    bool set_acr(boost::uint8_t acr, boost::uint8_t acr0, boost::uint8_t acr1, boost::uint8_t acr2, boost::uint8_t acr3)
    {
        boost::array<boost::uint8_t, 255 > buff;
        buff[0] = 0x20 | 6;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = SET_CANACR;
        buff[4] = acr;
        buff[5] = acr0;
        buff[6] = acr1;
        buff[7] = acr2;
        buff[8] = acr3;

        if (send_cmd(buff.data(), 9) != 9)
        {
            return false;
        }

        if (read_cmd(buff.data(), buff.size()) <= 0)
        {
            return false;
        }

        if (buff[4] != SJA_OK)
        {
            return false;
        }

        return true;
    }

    bool set_amr(boost::uint8_t amr, boost::uint8_t amr0, boost::uint8_t amr1, boost::uint8_t amr2, boost::uint8_t amr3)
    {
        boost::array<boost::uint8_t, 255 > buff;
        buff[0] = 0x20 | 6;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = SET_CANAMR;
        buff[4] = amr;
        buff[5] = amr0;
        buff[6] = amr1;
        buff[7] = amr2;
        buff[8] = amr3;

        if (send_cmd(buff.data(), 9) != 9)
        {
            return false;
        }

        if (read_cmd(buff.data(), buff.size()) <= 0)
        {
            return false;
        }

        if (buff[4] != SJA_OK)
        {
            return false;
        }

        return true;
    }

    std::size_t read_one_frame(boost::uint8_t *data, std::size_t maxLength,
                               bool isFinish, boost::system::error_code &ec)
    {
        static std::size_t length;
        static std::size_t mode;
        boost::array<boost::uint8_t, 16 > temp;

        if (_box->read_bytes(temp.data(), 3) != 3)
        {
            finish_execute(isFinish);
            ec = boost::asio::error::timed_out;
            return 0;
        }

        length = temp[0] & 0x0F;
        mode = temp[0] & (ExtID | RemoteFrame);

        switch (mode)
        {
        case StdID | DataFrame:
        case StdID | RemoteFrame:
            length += STD_FRAMEID_LENGTH;
            break;
        case ExtID | DataFrame:
        case ExtID | RemoteFrame:
            length += EXT_FRAMEID_LENGTH;
            break;
        default:
            finish_execute(isFinish);
            ec = boost::system::error_code(boost::system::errc::bad_message,
                                           boost::system::get_system_category());
            return 0;
            break;
        }

        length -= 2;
        if (length <= 0)
        {
            finish_execute(isFinish);
            ec = boost::asio::error::message_size;
            return 0;
        }
        else
        {
            if (_box->read_bytes(temp.data() + 3, length) != length)
            {
                ec = boost::asio::error::timed_out;
                finish_execute(isFinish);
                return 0;
            }
            finish_execute(isFinish);
            boost::array<boost::uint8_t, 8 > buff;
            return unpack(temp.data(), length + 3, data, maxLength, ec);
        }

        ec = boost::asio::error::message_size;
        return 0;
    }

private:
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<DefaultFunc<BoxType, ISO15765<BoxType> > > _default;
};
}
}
}

#endif
