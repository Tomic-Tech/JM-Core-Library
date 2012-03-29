#ifndef __JM_V1_ISO14230_HPP__
#define __JM_V1_ISO14230_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmtimer.h"
#include "jmutils.h"
#include "jmkwp2000.hpp"
#include "jmv1default.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BoxType>
        class ISO14230 : public JM::KWP2000
        {
        public:
            ISO14230(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &shared)
                : _box(box)
                , _shared(shared)
                , _default()
                , _lLine(false)
                , _sendLine(0)
                , _recvLine(0)
            {
                _default.reset(new JM::V1::Default<BoxType, JM::V1::ISO14230<BoxType> >(_box, _shared, this));
            }

            ~ISO14230()
            {
            }

            boost::int32_t fastInit(const boost::uint8_t *data, std::size_t count)
            {
                boost::uint8_t valueOpen = 0;
                boost::array<boost::uint8_t, 255> packEnter;
                std::size_t length;

                if (_lLine)
                {
                    valueOpen = BoxType::Constant::PWC | BoxType::Constant::RZFC | BoxType::Constant::CK;
                }
                else
                {
                    valueOpen = BoxType::Constant::PWC | BoxType::Constant::RZFC | BoxType::Constant::CK;
                }

                if (!_box->setCommCtrl(valueOpen, BoxType::Constant::SET_NULL) ||
                    !_box->setCommLine(_sendLine, _recvLine) ||
                    !_box->setCommLink(BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK | BoxType::Constant::SEL_SL | BoxType::Constant::UN_DB20, BoxType::Constant::SET_NULL, BoxType::Constant::SET_NULL) ||
                    !_box->setCommBaud(10416) ||
                    !_box->setCommTime(BoxType::Constant::SETBYTETIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(5))) ||
                    !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(0))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECBBOUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(400))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECFROUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(500))) ||
                    !_box->setCommTime(BoxType::Constant::SETLINKTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(500))))
                {
                    return JM_ERROR_GENERIC;
                }

				BoxType::sleep(BoxType::Seconds(1));

                _shared->buffID = 0;

                if (!_box->newBatch(_shared->buffID))
                {
                    return JM_ERROR_GENERIC;
                }

                length = pack(data, count, packEnter.data(), packEnter.size());

                if (!_box->setLineLevel(BoxType::Constant::COMS, BoxType::Constant::SET_NULL) ||
                    !_box->commboxDelay(BoxType::toMicroSeconds(BoxType::MilliSeconds(25))) ||
                    !_box->setLineLevel(BoxType::Constant::SET_NULL, BoxType::Constant::COMS) ||
                    !_box->commboxDelay(BoxType::toMicroSeconds(BoxType::MilliSeconds(25))) ||
                    !_box->sendOutData(packEnter.data(), length) ||
                    !_box->runReceive(BoxType::Constant::REC_FR) ||
                    !_box->endBatch())
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                if (!_box->runBatch(&(_shared->buffID), 1, false))
                    return JM_ERROR_GENERIC;

                length = readOneFrame(packEnter.data(), packEnter.size());
                if (length <= 0)
                    return JM_ERROR_GENERIC;

                _box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(55)));
                return JM_ERROR_SUCCESS;
            }

            void finishExecute(bool isFinish)
            {
                if (isFinish)
                {
                    _box->stopNow(true);
                    _box->delBatch(_shared->buffID);
                    _box->checkResult(BoxType::toMicroSeconds(BoxType::MilliSeconds(500)));
                }
            }

            boost::int32_t addrInit(boost::uint8_t addrCode)
            {
                boost::array<boost::uint8_t, 3> temp;
                if (!_box->setCommCtrl(BoxType::Constant::PWC | BoxType::Constant::REFC |
                    BoxType::Constant::RZFC | BoxType::Constant::CK, BoxType::Constant::SET_NULL) ||
                    !_box->setCommLink(BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK |
                    BoxType::Constant::SEL_SL | BoxType::Constant::SET_DB20, BoxType::Constant::SET_NULL,
                    BoxType::Constant::INVERTBYTE) ||
                    !_box->setCommBaud(5) ||
                    !_box->setCommTime(BoxType::Constant::SETBYTETIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(5))) ||
                    !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(12))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECBBOUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(400))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECFROUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(500))) ||
                    !_box->setCommTime(BoxType::Constant::SETLINKTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(500))))
                {
                    return JM_ERROR_GENERIC;
                }

				BoxType::sleep(BoxType::Seconds(1));

                _shared->buffID = 0;

                if (!_box->newBatch(_shared->buffID))
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->sendOutData(&addrCode, 1) ||
                    !_box->setCommLine((_recvLine == BoxType::Constant::RK_NO) ? _sendLine : BoxType::Constant::SK_NO, _recvLine) ||
                    !_box->runReceive(BoxType::Constant::SET55_BAUD) ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BoxType::Constant::REC_LEN_1) ||
                    !_box->endBatch())
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                if (!_box->runBatch(&(_shared->buffID), 1, false) ||
                    (_box->readData(temp.data(), temp.size(), BoxType::toMicroSeconds(BoxType::Seconds(3))) <= 0) ||
                    !_box->checkResult(BoxType::toMicroSeconds(BoxType::Seconds(5))))
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                if (!_box->delBatch(_shared->buffID) ||
                    !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(55))))
                {
                    return JM_ERROR_GENERIC;
                }

                if (temp[2] != 0)
                    return JM_ERROR_GENERIC;

                return JM_ERROR_SUCCESS;
            }

            boost::int32_t setLines(boost::int32_t comLine, bool lLine)
            {
                // According the connector to determine the send and receive line
                if (_shared->connector == JM_CN_OBDII_16)
                {
                    switch (comLine)
                    {
                    case 7:
                        _sendLine = BoxType::Constant::SK1;
                        _recvLine = BoxType::Constant::RK1;
                        break;
                    default:
                        return JM_ERROR_GENERIC;
                    }
                }
                else
                {
                    return JM_ERROR_GENERIC;
                }
                _lLine = lLine;
                return JM_ERROR_SUCCESS;
            }

            std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count)
            {
                return sendOneFrame(data, count, false);
            }

            std::size_t sendFrames(const boost::uint8_t *data, std::size_t count)
            {
                return sendOneFrame(data, count);
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
				boost::array<boost::uint8_t, 255> buff;
                std::size_t length;
                _mode = _linkMode;
                length = pack(data, count, buff.data(), buff.size());
                _mode = _msgMode;
                return _default->setKeepLink(buff.data(), length);
            }
        private:
            std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength, bool isFinish)
            {
                static boost::array<boost::uint8_t, 3> temp;
                static boost::array<boost::uint8_t, 255> result;
                std::size_t frameLength = 0;
                std::size_t length;
                boost::uint8_t checksum;
                std::size_t i;

                length = _box->readBytes(temp.data(), temp.size());

                if (length <= 0)
                {
                    finishExecute(isFinish);
                    return 0;
                }

                if (temp[1] == _sourceAddress)
                {
                    if (temp[0] == 0x80)
                    {
                        boost::uint8_t b;
                        length = _box->readBytes(&b, 1);
                        if (length <= 0)
                        {
                            finishExecute(isFinish);
                            return 0;
                        }
                        length = b;
                        if (length <= 0)
                        {
                            finishExecute(isFinish);
                            return 0;
                        }
                        memcpy(result.data(), temp.data(), temp.size());
                        frameLength += temp.size();
                        memcpy(result.data() + frameLength, &b, 1);
                        frameLength++;
                        length = _box->readBytes(result.data() + KWP80_HEADER_LENGTH, length + KWP_CHECKSUM_LENGTH);
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
                        length = _box->readBytes(result.data() + temp.size(), length + KWP_CHECKSUM_LENGTH);
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
                            finishExecute(isFinish);
                            return 0;
                        }
                        memcpy(result.data(), temp.data(), temp.size());
                        frameLength += temp.size();
                        length = _box->readBytes(result.data() + temp.size(), length);
                        frameLength += length;
                    }
                    else
                    {
                        length = temp[0];
                        if (length <= 0)
                        {
                            finishExecute(isFinish);
                            return 0;
                        }
                        memcpy(result.data(), temp.data(), temp.size());
                        frameLength += temp.size();
                        length = _box->readBytes(result.data() + temp.size(), length - KWP_CHECKSUM_LENGTH);
                        frameLength += length;
                    }
                }

                finishExecute(isFinish);
                if (frameLength <= 0)
                {
                    return frameLength;
                }

                checksum = 0;
                for (i = 0; i < frameLength - 1; i++)
                {
                    checksum += result[i];
                }

                if (checksum != result[frameLength - 1])
                {
                    return 0;
                }

                return unpack(result.data(), frameLength, data, maxLength);
            }

            boost::int32_t sendOneFrame(const boost::uint8_t *data, std::size_t count, bool isFinish)
            {
                return _default->sendOneFrame(data, count, isFinish);
            }
        private:
            boost::shared_ptr<BoxType> _box;
            boost::shared_ptr<Shared> _shared;
            boost::shared_ptr<Default<BoxType, JM::V1::ISO14230<BoxType> > >_default;
            bool _lLine;
            boost::uint8_t _sendLine;
            boost::uint8_t _recvLine;
        };
    }
}

#endif