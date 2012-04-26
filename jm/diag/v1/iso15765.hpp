#ifndef __JM_DIAG_V1_ISO15765_HPP__
#define __JM_DIAG_V1_ISO15765_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/cstdint.hpp>
#include <jm/utils.hpp>
#include <jm/diag/canbus.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
static const boost::uint8_t SJA_BTR_CODETABLE[30] =
{
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
    ISO15765(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &shared)
        : _box(box)
        , _shared(shared)
        , _default()
    {
        _default.reset(new Default<BoxType, ISO15765<BoxType> >(_box, _shared, this));
    }

    ~ISO15765()
    {
    }

    void finishExecute(bool isFinish)
    {
        if (isFinish)
        {
            _box->stopNow(false);
            _box->delBatch(_shared->buffID);
            _box->checkResult(BoxType::toMicroSeconds(BoxType::MilliSeconds(200)));
        }
    }

    void setLines(boost::int32_t high, boost::int32_t low,
                  boost::system::error_code &ec)
    {
        return Canbus::setLines(high, low, ec);
    }

    void setFilter(const std::vector<boost::int32_t> &idVector,
                   boost::system::error_code &ec)
    {
        return Canbus::setFilter(idVector, ec);
    }

    void setOptions(boost::int32_t id, Baud baud,
                    IDMode idMode,
                    FilterMask mask,
                    FrameType frame,
                    boost::system::error_code &ec)
    {
        Canbus::setOptions(id, baud, idMode, mask, frame, ec);
        if (ec)
        {
            return;
        }
        _targetID = (_targetID << 5) & 0x0000FFFF;
    }

    inline bool reset()
    {
        return doCmd(SET_CANRESET, 0);
    }

    void init(boost::system::error_code &ec)
    {
        boost::uint32_t localID = 0;
        boost::uint8_t ctrlWord1;

        ctrlWord1 = BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK | BoxType::Constant::SEL_SL | BoxType::Constant::UN_DB20;

        if (!_box->setCommCtrl(BoxType::Constant::RZFC, BoxType::Constant::SET_NULL) ||
                !_box->setCommLine(BoxType::Constant::SK5, BoxType::Constant::SK7) ||
                !_box->setCommLink(ctrlWord1, BoxType::Constant::SET_NULL, BoxType::Constant::SET_NULL) ||
                !_box->setCommBaud(57600) ||
                !_box->setCommTime(BoxType::Constant::SETBYTETIME, 500) ||
                !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(55))) ||
                !_box->setCommTime(BoxType::Constant::SETRECBBOUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(100))) ||
                !_box->setCommTime(BoxType::Constant::SETRECFROUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(200))) ||
                !_box->setCommTime(BoxType::Constant::SETLINKTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(500))))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }
        localID = _idVector[0];

        BoxType::sleep(BoxType::MilliSeconds(100));

        if (!beginSet() ||
                !setBaud(B500K) ||
                !setAcr(CAN_ACR_ACF1,
                        HighByte(HighWord(localID << 21)),
                        LowByte(HighWord(localID << 21)),
                        HighByte(LowWord(localID << 21)),
                        LowByte(LowWord(localID << 21))) ||
                !setAmr(CAN_AMR_ACF1, 0x00, 0x1F, 0xFF, 0xFF) ||
                !setMode(0x55) ||
                !setPrior(0xFF) ||
                !setChan(ACF1_FT1_CHAN) ||
                !endSet())
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        static boost::uint8_t keepLink[2] = {0x3E, 0x01};
        setKeepLink(keepLink, 2, ec);
        ec = boost::system::error_code();
    }

    std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count,
                             bool isFinish, boost::system::error_code &ec)
    {
        return _default->sendOneFrame(data, count, isFinish, ec);
    }

    std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count,
                             boost::system::error_code &ec)
    {
        return sendOneFrame(data, count, false, ec);
    }

    std::size_t sendFrames(const boost::uint8_t *data, std::size_t count,
                           boost::system::error_code &ec)
    {
        if (count <= 0 || count > 0x0FFF)
        {
            ec = boost::asio::error::message_size;
            return 0;
        }

        if (count < 8)
        {
            boost::array<boost::uint8_t, 8> temp;
            memset(temp.data(), 0, temp.size());

            temp[0] = LowByte(count);
            memcpy(temp.data() + 1, data, count);
            return sendOneFrame(temp.data(), temp.size(), true, ec);
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

            for(; frame_index < frame_count; ++frame_index)
            {
                if (0 == frame_index)
                {
                    boost::array<boost::uint8_t, 8> temp;
                    temp[0] = (0x10 | HighByte(LowWord(count)));
                    temp[1] = LowByte(LowWord(count));
                    memcpy(temp.data() + 2, data + pos, 6);
                    pos += 6;
                    sendOneFrame(temp.data(), temp.size(), true, ec);
                    if (ec)
                    {
                        return 0;
                    }
                    readOneFrame(temp.data(), temp.size(), ec);
                    if (ec)
                    {
                        return 0;
                    }
                }
                else if (frame_index == (frame_count - 1))
                {
                    boost::array<boost::uint8_t, 8> temp;
                    memset(temp.data(), 0, temp.size());
                    temp[0] = seq;

                    memcpy(temp.data() + 1, data + pos, count - pos);
                    if (sendOneFrame(temp.data(), temp.size(), true, ec) != (count - pos + 1))
                        return 0;
                    ec = boost::system::error_code();
                    return count;
                }
                else
                {
                    boost::array<boost::uint8_t, 8> temp;
                    temp[0] = seq;
                    memcpy(temp.data() + 1, data + pos, 7);
                    pos += 7;
                    if (sendOneFrame(temp.data(), temp.size(), false, ec) != 8)
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

    std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength,
                             boost::system::error_code &ec)
    {
        return readOneFrame(data, maxLength, true, ec);
    }

    std::size_t readFrames(boost::uint8_t *data, std::size_t maxLength,
                           boost::system::error_code &ec)
    {
        boost::array<boost::uint8_t, 8> firstFrame;
        std::size_t length;

        length = readOneFrame(firstFrame.data(), firstFrame.size(), false, ec);

        if (ec)
        {
            return 0;
        }

        // Check if the flow control
        if (firstFrame[0] == 0x30)
        {
            finishExecute(true);
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
            static boost::array<boost::uint8_t, 0xFFF> restFrames;
            static boost::array<boost::uint8_t, 8> unpackedBuff;

            finishExecute(true);

            if (sendOneFrame(_flowControl.data(), _flowControl.size(), true, ec) != 8)
            {
                return 0;
            }

            pos = 0;
            userDataCount = ((firstFrame[0] & 0x0F) << 8) | (firstFrame[1] & 0xFF);
            userDataCount -= 6;
            resetFrameCount = (userDataCount / 7) + ((userDataCount % 7) ? 1 : 0);
            memcpy(data + pos, firstFrame.data() + 2, length - 2);
            pos += length - 2;

            if (_idMode == StdID)
            {
                dataLength = STD_FRAMEID_LENGTH + 8;
            }
            else
            {
                dataLength = EXT_FRAMEID_LENGTH + 8;
            }

            restDataCount = dataLength * resetFrameCount;

            if (_box->readBytes(restFrames.data(), restDataCount) != restDataCount)
            {
                finishExecute(true);
                ec = boost::asio::error::timed_out;
                return 0;
            }

            finishExecute(true);
            for (i = 0; i < resetFrameCount; ++i)
            {
                // In ISO15765, we always read 8 bytes data.
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
                length = readOneFrame(firstFrame.data(), firstFrame.size(), false, ec);
            }
            finishExecute(true);
            memcpy(data, firstFrame.data() + 1, length - 1);
            return length;
        }
        ec = boost::asio::error::timed_out;
        return 0;
    }

    void setKeepLink(const boost::uint8_t *data, std::size_t count,
                     boost::system::error_code &ec)
    {
        boost::array<boost::uint8_t, 255> buff;
        std::size_t length;

        length = pack(data, count, buff.data(), buff.size(), ec);

        _default->setKeepLink(buff.data(), length, ec);
    }
private:
    std::size_t sendCmd(const boost::uint8_t *data, std::size_t count)
    {
        _shared->buffID = 0;
        if (!_box->newBatch(_shared->buffID))
        {
            return 0;
        }

        if (!_box->sendOutData(data, count) ||
                !_box->runReceive(BoxType::Constant::RECEIVE) ||
                !_box->endBatch() ||
                !_box->runBatch(&_shared->buffID, 1, false))
        {
            _box->delBatch(_shared->buffID);
            return 0;
        }

        BoxType::sleep(BoxType::MicroSeconds(count * _shared->reqByteToByte + _shared->reqWaitTime));
        return count;
    }

    std::size_t readCmd(boost::uint8_t *buff, std::size_t maxLength)
    {
        std::size_t length;

        if (_box->readBytes(buff, 3) != 3)
        {
            finishExecute(true);
            return 0;
        }

        length = buff[0] & 0x0F;

        switch(buff[0] & 0xC0)
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
            finishExecute(true);
            return 0;
        }

        length -= 2;
        if (length > 0)
        {
            if (_box->readBytes(buff + 3, length) != length)
            {
                finishExecute(true);
                return 0;
            }
        }
        finishExecute(true);
        return length + 3;
    }

    bool doCmd(boost::uint8_t cmd, boost::uint8_t inf)
    {
        boost::array<boost::uint8_t, 255> buff;
        buff[0] = 0x20 | 2;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = cmd;
        buff[4] = inf;

        std::size_t length;

        if (sendCmd(buff.data(), 5) != 5)
        {
            return false;
        }
        length = readCmd(buff.data(), buff.size());

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

    inline bool beginSet()
    {
        return doCmd(SET_CANBEGIN, 0);
    }

    inline bool setMode(boost::uint8_t mode)
    {
        return doCmd(SET_CANMODE, mode);
    }

    inline bool setPrior(boost::uint8_t prior)
    {
        return doCmd(SET_CANPRIOR, prior);
    }

    inline bool setChan(boost::uint8_t chan)
    {
        return doCmd(SET_CANCHAN, chan);
    }

    inline bool endSet()
    {
        return doCmd(SET_CANEND, 0);
    }

    bool setBaud(Baud baud)
    {
        boost::array<boost::uint8_t, 255> buff;
        buff[0] = 0x20 | 3;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = SET_CANBAUD;

        switch(baud)
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

        if (sendCmd(buff.data(), 6) != 6)
        {
            return false;
        }

        if (readCmd(buff.data(), buff.size()) <= 0)
        {
            return false;
        }

        if (buff[4] != SJA_OK)
        {
            return false;
        }

        return true;
    }

    bool setAcr(boost::uint8_t acr, boost::uint8_t acr0, boost::uint8_t acr1, boost::uint8_t acr2, boost::uint8_t acr3)
    {
        boost::array<boost::uint8_t, 255> buff;
        buff[0] = 0x20 | 6;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = SET_CANACR;
        buff[4] = acr;
        buff[5] = acr0;
        buff[6] = acr1;
        buff[7] = acr2;
        buff[8] = acr3;

        if (sendCmd(buff.data(), 9) != 9)
        {
            return false;
        }

        if (readCmd(buff.data(), buff.size()) <= 0)
        {
            return false;
        }

        if (buff[4] != SJA_OK)
        {
            return false;
        }

        return true;
    }

    bool setAmr(boost::uint8_t amr, boost::uint8_t amr0, boost::uint8_t amr1, boost::uint8_t amr2, boost::uint8_t amr3)
    {
        boost::array<boost::uint8_t, 255> buff;
        buff[0] = 0x20 | 6;
        buff[1] = 0x55;
        buff[2] = 0xAA;
        buff[3] = SET_CANAMR;
        buff[4] = amr;
        buff[5] = amr0;
        buff[6] = amr1;
        buff[7] = amr2;
        buff[8] = amr3;

        if (sendCmd(buff.data(), 9) != 9)
        {
            return false;
        }

        if (readCmd(buff.data(), buff.size()) <= 0)
        {
            return false;
        }

        if (buff[4] != SJA_OK)
        {
            return false;
        }

        return true;
    }

    std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength,
                             bool isFinish, boost::system::error_code &ec)
    {
        static std::size_t length;
        static std::size_t mode;
        boost::array<boost::uint8_t, 16> temp;

        if (_box->readBytes(temp.data(), 3) != 3)
        {
            finishExecute(isFinish);
            ec = boost::asio::error::timed_out;
            return 0;
        }

        length = temp[0] & 0x0F;
        mode = temp[0] & (ExtID | RemoteFrame);

        switch(mode)
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
            finishExecute(isFinish);
            ec = boost::system::error_code(boost::system::errc::bad_message,
                                           boost::system::get_system_category());
            return 0;
            break;
        }

        length -= 2;
        if (length <= 0)
        {
            finishExecute(isFinish);
            ec = boost::asio::error::message_size;
            return 0;
        }
        else
        {
            if (_box->readBytes(temp.data() + 3, length) != length)
            {
                ec = boost::asio::error::timed_out;
                finishExecute(isFinish);
                return 0;
            }
            finishExecute(isFinish);
            boost::array<boost::uint8_t, 8> buff;
            return unpack(temp.data(), length + 3, data, maxLength, ec);
        }

        ec = boost::asio::error::message_size;
        return 0;
    }

private:
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<Default<BoxType, ISO15765<BoxType> > > _default;
};
}
}
}

#endif