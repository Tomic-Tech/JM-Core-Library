#ifndef __JM_V1_KWP1281_HPP__
#define __JM_V1_KWP1281_HPP__

#include "jmkwp1281.hpp"
#include "jmv1default.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BoxType>
        class KWP1281 : public JM::KWP1281
        {
        public:
            KWP1281(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &_shared)
                : _box(box)
                , _shared(_shared)
                , _default()
            {
                _default.reset(new JM::V1::Default<BoxType, JM::V1::KWP1281<BoxType> >(_box, _shared, this));
            }

            ~KWP1281()
            {
            }
            void finishExecute(bool isFinish)
            {
                if (isFinish)
                {
                    _box->delBatch(_shared->buffID);
                    _box->checkResult(BoxType::toMicroSeconds(BoxType::MilliSeconds(500)));
                }
            }

            boost::int32_t addrInit(boost::uint8_t addrCode)
            {
                bool dl0;
                boost::uint8_t temp[255];
                std::size_t length;
                boost::uint8_t valueOpen;
                boost::uint8_t valueClose;
                boost::uint8_t ctrlWord1;
                boost::uint8_t ctrlWord2;
                boost::uint8_t ctrlWord3;

                _shared->buffID = 0;
                _box->stopNow(true);
                _box->checkResult(BoxType::toMicroSeconds(BoxType::MilliSeconds(50)));

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
                    !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(25))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECBBOUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(610))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECFROUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(610))) ||
                    !_box->setCommTime(BoxType::Constant::SETLINKTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(710))))
                {
                    return JM_ERROR_GENERIC;
                }

				BoxType::sleep(BoxType::Seconds(1));

                if (!_box->newBatch(_shared->buffID))
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->sendOutData(&addrCode, 1) ||
                    !_box->setCommLine(_kLine == BoxType::Constant::RK1 ? BoxType::Constant::SK_NO : _lLine, _kLine) ||
                    !_box->runReceive(BoxType::Constant::SET55_BAUD) ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->setCommTime(BoxType::Constant::SETBYTETIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(2))) ||
                    !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(2))) ||
                    !_box->runReceive(BoxType::Constant::REC_FR) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->endBatch())
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }
                if (!_box->runBatch(&_shared->buffID, 1,  false))
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                if (_box->readData(temp, 2, BoxType::toMicroSeconds(BoxType::MilliSeconds(3500))) != 2)
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                length = readOneFrame(temp, 255);
                if (length <= 0)
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->checkResult(BoxType::toMicroSeconds(BoxType::MilliSeconds(500))))
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                if (!_box->delBatch(_shared->buffID))
                {
                    return JM_ERROR_GENERIC;
                }

                _frameCounter = temp[0];

                return JM_ERROR_SUCCESS;
            }

            boost::int32_t setLines(boost::int32_t comLine, bool lLine)
            {
                if (_shared->connector == JM_CN_AUDI_4 ||
                    _shared->connector == JM_CN_OBDII_16)
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
                    return JM_ERROR_GENERIC;
                }
                return JM_ERROR_SUCCESS;
            }

            std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count, bool needRecv)
            {
                boost::uint8_t sendBuff[255];
                std::size_t length;

                _shared->buffID = 0;

                if (!_box->newBatch(_shared->buffID))
                {
                    return 0;
                }

                length = pack(data, count, sendBuff, 255);

                if (length <= 0)
                {
                    return 0;
                }

                if (needRecv)
                {
                    if (!_box->turnOverOneByOne() ||
                        !_box->sendOutData(sendBuff, length) ||
                        !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BoxType::Constant::REC_FR) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                        !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BoxType::Constant::REC_FR) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, false))
                    {
                        _box->delBatch(_shared->buffID);
                        return 0;
                    }
                    g_usleep((length * _shared->reqByteToByte + 
                        _shared->reqWaitTime) / 1000);
                }
                else
                {
                    if (!_box->turnOverOneByOne() ||
                        !_box->sendOutData(sendBuff, length) ||
                        !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BoxType::Constant::REC_FR) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                        !_box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, false))
                    {
                        _box->delBatch(_shared->buffID);
                        return 0;
                    }
                }
                if (_box->readBytes(sendBuff, length - 1) != (length - 1))
                {
                    return 0;
                }
                return count;
            }

            std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count)
            {
                return sendOneFrame(data, count, false);
            }

            std::size_t sendFrames(const boost::uint8_t *data, std::size_t count)
            {
                return sendOneFrame(data, count);
            }

            std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength, bool isFinish)
            {
                std::size_t pos = 0;
                std::size_t len;
                boost::uint8_t temp[255];

                if (_box->readBytes(temp, 1) != 1)
                {
                    return 0;
                }

                pos++;
                len = temp[0];

                if (_box->readBytes(temp + pos, len) != len)
                {
                    return 0;
                }

                pos += len;
                finishExecute(isFinish);

                return unpack(temp, pos, data, maxLength);
            }

            std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength)
            {
                return readOneFrame(data, maxLength, true);
            }

            std::size_t readFrames(boost::uint8_t *data, std::size_t maxLength)
            {
                return readOneFrame(data, maxLength);
            }

            boost::int32_t setKeepLink(const boost::uint8_t *data, std::size_t count)
            {
                boost::uint8_t add1 = 0;
                boost::uint8_t add2 = 0;
                boost::uint8_t tempBuff[3];

                _buffIDAddr[0] = BoxType::Constant::LINKBLOCK;
                if (_box->newBatch(BoxType::Constant::LINKBLOCK))
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->turnOverOneByOne() ||
                    !_box->sendOutData(data, data[0]))
                {
                    return JM_ERROR_GENERIC;
                }

                _buffIDAddr[1] = data[0] - 2;

                if (!_box->turnOverOneByOne() ||
                    !_box->sendOutData(data, 1) ||
                    !((add1 = _box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr)) == 0) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_FR) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !((add2 = _box->updateBuff(BoxType::Constant::INC_DATA, _buffIDAddr)) == 0) ||
                    !_box->endBatch())
                {
                    return JM_ERROR_GENERIC;
                }

                tempBuff[0] = BoxType::Constant::LINKBLOCK;
                tempBuff[1] = add1;
                tempBuff[2] = _buffIDAddr[1];

                if (!_box->getAbsAdd(BoxType::Constant::LINKBLOCK, tempBuff[2]))
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->updateBuff(BoxType::Constant::UPDATE_1BYTE, tempBuff))
                {
                    return JM_ERROR_GENERIC;
                }

                tempBuff[1] = add2;

                if (!_box->updateBuff(BoxType::Constant::UPDATE_1BYTE, tempBuff))
                {
                    return JM_ERROR_GENERIC;
                }

                return JM_ERROR_SUCCESS;
            }
        private:
            boost::shared_ptr<BoxType> _box;
            boost::shared_ptr<Shared> _shared;
            boost::shared_ptr<Default<BoxType, JM::V1::KWP1281<BoxType> > >_default;
            boost::uint8_t _buffIDAddr[2]; // 0 == buffID, 1 == buffAddr
            boost::uint8_t _kLine;
            boost::uint8_t _lLine;
        };
    }
}

#endif

