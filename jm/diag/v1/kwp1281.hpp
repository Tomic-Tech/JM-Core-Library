#ifndef __DIAG_V1_KWP1281_HPP__
#define __DIAG_V1_KWP1281_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/utils.hpp>
#include <jm/diag/kwp1281.hpp>
#include <jm/diag/v1/default.hpp>

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
    KWP1281(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &_shared)
        : _box(box)
        , _shared(_shared)
        , _default()
    {
        _default.reset(new Default<BoxType, KWP1281<BoxType> >(_box, _shared, this));
    }

    ~KWP1281()
    {
    }
    void finishExecute(bool isFinish)
    {
        if (isFinish)
        {
            _box->delBatch(_shared->buffID);
            _box->checkResult(BoxType::toMicroSeconds(typename BoxType::MilliSeconds(500)));
        }
    }

    void addrInit(boost::uint8_t addrCode, boost::system::error_code &ec)
    {
        bool dl0;
        boost::array<boost::uint8_t, 255> temp;
        std::size_t length;
        boost::uint8_t valueOpen;
        boost::uint8_t valueClose;
        boost::uint8_t ctrlWord1;
        boost::uint8_t ctrlWord2;
        boost::uint8_t ctrlWord3;

        _shared->buffID = 0;
        _box->stopNow(true);
        _box->checkResult(BoxType::toMicroSeconds(typename BoxType::MilliSeconds(50)));

        dl0 = (_lLine == BoxType::Constant::SK_NO && _kLine == BoxType::Constant::RK_NO) ? true : false;
        valueOpen = BoxType::Constant::PWC | BoxType::Constant::REFC | BoxType::Constant::RZFC | (dl0 ? BoxType::Constant::DLC0 : BoxType::Constant::CK);
        valueClose = BoxType::Constant::SET_NULL;
        ctrlWord1 = BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK | (dl0 ? BoxType::Constant::SEL_DL0 : BoxType::Constant::SEL_SL) | BoxType::Constant::SET_DB20;
        ctrlWord2 = 0xFF;
        ctrlWord3 = BoxType::Constant::INVERTBYTE | 1;

        if (!_box->setCommCtrl(valueOpen, valueClose) ||
                !_box->setCommLine(_lLine, _kLine) ||
                !_box->setCommLink(ctrlWord1, ctrlWord2, ctrlWord3) ||
                !_box->setCommBaud(5) ||
                !_box->setCommTime(BoxType::Constant::SETBYTETIME, 0) ||
                !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(typename BoxType::MilliSeconds(25))) ||
                !_box->setCommTime(BoxType::Constant::SETRECBBOUT, BoxType::toMicroSeconds(typename BoxType::MilliSeconds(610))) ||
                !_box->setCommTime(BoxType::Constant::SETRECFROUT, BoxType::toMicroSeconds(typename BoxType::MilliSeconds(610))) ||
                !_box->setCommTime(BoxType::Constant::SETLINKTIME, BoxType::toMicroSeconds(typename BoxType::MilliSeconds(710))))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        BoxType::sleep(typename BoxType::Seconds(1));

        if (!_box->newBatch(_shared->buffID))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        if (!_box->sendOutData(&addrCode, 1) ||
                !_box->setCommLine(_kLine == BoxType::Constant::RK1 ? BoxType::Constant::SK_NO : _lLine, _kLine) ||
                !_box->runReceive(BoxType::Constant::SET55_BAUD) ||
                !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                !_box->turnOverOneByOne() ||
                !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                !_box->setCommTime(BoxType::Constant::SETBYTETIME, BoxType::toMicroSeconds(typename BoxType::MilliSeconds(2))) ||
                !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(typename BoxType::MilliSeconds(2))) ||
                !_box->runReceive(BoxType::Constant::REC_FR) ||
                !_box->turnOverOneByOne() ||
                !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                !_box->endBatch())
        {
            _box->delBatch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return;
        }
        if (!_box->runBatch(&_shared->buffID, 1,  false))
        {
            _box->delBatch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return;
        }

        if (_box->readData(temp.data(), 2, BoxType::toMicroSeconds(typename BoxType::MilliSeconds(3500))) != 2)
        {
            _box->delBatch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return;
        }

        length = readOneFrame(temp.data(), temp.size(), ec);
        if (ec)
        {
            return;
        }

        if (!_box->checkResult(BoxType::toMicroSeconds(typename BoxType::MilliSeconds(500))))
        {
            _box->delBatch(_shared->buffID);
            ec = boost::asio::error::connection_refused;
            return;
        }

        if (!_box->delBatch(_shared->buffID))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        _frameCounter = temp[0];
    }

    void setLines(boost::int32_t comLine, bool lLine,
                  boost::system::error_code &ec)
    {
        if (_shared->connector == CN_AUDI_4 ||
                _shared->connector == CN_OBDII_16)
        {
            if (comLine != 0)
            {
                _kLine = BoxType::Constant::RK1;
            }
            else
            {
                _kLine = BoxType::Constant::RK_NO;
            }
            if (lLine)
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
        }
        ec = boost::system::error_code();
    }

    std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count,
                             bool needRecv, boost::system::error_code &ec)
    {
        boost::array<boost::uint8_t, 255> sendBuff;
        std::size_t length;

        _shared->buffID = 0;

        if (!_box->newBatch(_shared->buffID))
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
            if (!_box->turnOverOneByOne() ||
                    !_box->sendOutData(sendBuff.data(), length) ||
                    !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_FR) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_FR) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->endBatch() ||
                    !_box->runBatch(&_shared->buffID, 1, false))
            {
                _box->delBatch(_shared->buffID);
                ec = boost::asio::error::connection_refused;
                return 0;
            }
            boost::this_thread::sleep(boost::posix_time::microseconds((length * _shared->reqByteToByte + _shared->reqWaitTime) / 1000));

        }
        else
        {
            if (!_box->turnOverOneByOne() ||
                    !_box->sendOutData(sendBuff.data(), length) ||
                    !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_FR) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr.data()) ||
                    !_box->endBatch() ||
                    !_box->runBatch(&_shared->buffID, 1, false))
            {
                _box->delBatch(_shared->buffID);
                ec = boost::asio::error::connection_refused;
                return 0;
            }
        }
        if (_box->readBytes(sendBuff.data(), length - 1) != (length - 1))
        {
            ec = boost::asio::error::timed_out;
            return 0;
        }
        return count;
    }

    std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count,
                             boost::system::error_code &ec)
    {
        return sendOneFrame(data, count, false, ec);
    }

    std::size_t sendFrames(const boost::uint8_t *data, std::size_t count,
                           boost::system::error_code &ec)
    {
        return sendOneFrame(data, count, ec);
    }

    std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength,
                             bool isFinish, boost::system::error_code &ec)
    {
        std::size_t pos = 0;
        std::size_t len;
        boost::array<boost::uint8_t, 255> temp;

        if (_box->readBytes(temp.data(), 1) != 1)
        {
            ec = boost::asio::error::timed_out;
            return 0;
        }

        ++pos;
        len = temp[0];

        if (_box->readBytes(temp.data() + pos, len) != len)
        {
            ec = boost::asio::error::timed_out;
            return 0;
        }

        pos += len;
        finishExecute(isFinish);

        return unpack(temp.data(), pos, data, maxLength, ec);
    }

    std::size_t readOneFrame(boost::uint8_t *data,
                             std::size_t maxLength, boost::system::error_code &ec)
    {
        return readOneFrame(data, maxLength, true, ec);
    }

    std::size_t readFrames(boost::uint8_t *data, std::size_t maxLength,
                           boost::system::error_code &ec)
    {
        return readOneFrame(data, maxLength, ec);
    }

    void setKeepLink(const boost::uint8_t *data, std::size_t count,
                     boost::system::error_code &ec)
    {
        boost::uint8_t add1 = 0;
        boost::uint8_t add2 = 0;
        boost::array<boost::uint8_t, 3> tempBuff;

        _buffIDAddr[0] = BoxType::Constant::LINKBLOCK;
        if (_box->newBatch(BoxType::Constant::LINKBLOCK))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        if (!_box->turnOverOneByOne() ||
                !_box->sendOutData(data, data[0]))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        _buffIDAddr[1] = data[0] - 2;

        if (!_box->turnOverOneByOne() ||
                !_box->sendOutData(data, 1) ||
                !((add1 = _box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr.data())) == 0) ||
                !_box->turnOverOneByOne() ||
                !_box->runReceive(BoxType::Constant::REC_FR) ||
                !_box->turnOverOneByOne() ||
                !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                !((add2 = _box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr.data())) == 0) ||
                !_box->endBatch())
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        tempBuff[0] = BoxType::Constant::LINKBLOCK;
        tempBuff[1] = add1;
        tempBuff[2] = _buffIDAddr[1];

        if (!_box->getAbsAdd(BoxType::Constant::LINKBLOCK, tempBuff[2]))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        if (!_box->updateBuff(BoxType::Constant::UPDATE_1BYTE, tempBuff.data()))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        tempBuff[1] = add2;

        if (!_box->updateBuff(BoxType::Constant::UPDATE_1BYTE, tempBuff.data()))
        {
            ec = boost::asio::error::connection_refused;
            return;
        }

        ec = boost::system::error_code();
    }
private:
    boost::shared_ptr<BoxType> _box;
    boost::shared_ptr<Shared> _shared;
    boost::shared_ptr<Default<BoxType, KWP1281<BoxType> > >_default;
    boost::array<boost::uint8_t, 2> _buffIDAddr; // 0 == buffID, 1 == buffAddr
    boost::uint8_t _kLine;
    boost::uint8_t _lLine;
};
}
}
}

#endif
