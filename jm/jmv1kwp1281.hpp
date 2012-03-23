#ifndef __JM_V1_KWP1281_HPP__
#define __JM_V1_KWP1281_HPP__

#include "jmkwp1281.hpp"
#include "jmv1default.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BOX>
        class KWP1281 : public JM::KWP1281
        {
        public:
            KWP1281(BOX *box, Shared *_shared)
                : _box(box)
                , _shared(_shared)
                , _default(NULL)
            {
                _default = new JM::V1::Default<BOX, JM::V1::KWP1281<BOX> >(_box, _shared, this);
            }

            ~KWP1281()
            {
                delete _default;
            }
            void finishExecute(gboolean isFinish)
            {
                if (isFinish)
                {
                    _box->delBatch(_shared->buffID);
                    _box->checkResult(JM_TIMER_TO_MS(500));
                }
            }

            gint32 addrInit(guint8 addrCode)
            {
                gboolean dl0;
                guint8 temp[256];
                size_t length;
                guint8 valueOpen;
                guint8 valueClose;
                guint8 ctrlWord1;
                guint8 ctrlWord2;
                guint8 ctrlWord3;

                _shared->buffID = 0;
                _box->stopNow(TRUE);
                _box->checkResult(JM_TIMER_TO_MS(50));

                dl0 = (_lLine == BOX::C::SK_NO && 
                    _kLine == BOX::C::RK_NO) ? TRUE : FALSE;
                valueOpen = BOX::C::PWC | BOX::C::REFC | BOX::C::RZFC | (dl0 ? BOX::C::DLC0 : BOX::C::CK);
                valueClose = BOX::C::SET_NULL;
                ctrlWord1 = BOX::C::RS_232 | BOX::C::BIT9_MARK | (dl0 ? BOX::C::SEL_DL0 : BOX::C::SEL_SL) | BOX::C::SET_DB20;
                ctrlWord2 = 0xFF;
                ctrlWord3 = BOX::C::INVERTBYTE | 1;

                if (!_box->setCommCtrl(valueOpen, valueClose) ||
                    !_box->setCommLine(_lLine, _kLine) ||
                    !_box->setCommLink(ctrlWord1, ctrlWord2, ctrlWord3) ||
                    !_box->setCommBaud(5) ||
                    !_box->setCommTime(BOX::C::SETBYTETIME, 0) ||
                    !_box->setCommTime(BOX::C::SETWAITTIME, JM_TIMER_TO_MS(25)) ||
                    !_box->setCommTime(BOX::C::SETRECBBOUT, JM_TIMER_TO_MS(610)) ||
                    !_box->setCommTime(BOX::C::SETRECFROUT, JM_TIMER_TO_MS(610)) ||
                    !_box->setCommTime(BOX::C::SETLINKTIME, JM_TIMER_TO_MS(710)))
                {
                    return JM_ERROR_GENERIC;
                }

                g_usleep(JM_TIMER_TO_MS(1));

                if (!_box->newBatch(_shared->buffID))
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->sendOutData(&addrCode, 1) ||
                    !_box->setCommLine(_kLine == BOX::C::RK1 ? BOX::C::SK_NO : _lLine, _kLine) ||
                    !_box->runReceive(BOX::C::SET55_BAUD) ||
                    !_box->runReceive(BOX::C::REC_LEN_1) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BOX::C::REC_LEN_1) ||
                    !_box->setCommTime(BOX::C::SETBYTETIME, JM_TIMER_TO_MS(2)) ||
                    !_box->setCommTime(BOX::C::SETWAITTIME, JM_TIMER_TO_MS(2)) ||
                    !_box->runReceive(BOX::C::REC_FR) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BOX::C::REC_LEN_1) ||
                    !_box->endBatch())
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }
                if (!_box->runBatch(&_shared->buffID, 1,  FALSE))
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                if (_box->readData(temp, 2, JM_TIMER_TO_MS(3500)) != 2)
                {
                    _box->delBatch(_shared->buffID);
                    return JM_ERROR_GENERIC;
                }

                length = readOneFrame(temp);
                if (length <= 0)
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->checkResult(JM_TIMER_TO_MS(500)))
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

            gint32 setLines(gint32 comLine, gboolean lLine)
            {
                if (_shared->connector == JM_CN_AUDI_4 ||
                    _shared->connector == JM_CN_OBDII_16)
                {
                    if (comLine != 0)
                    {
                        _kLine = BOX::C::RK1;
                    }
                    else
                    {
                        _kLine = BOX::C::RK_NO;
                    }
                    if (lLine)
                    {
                        _lLine = BOX::C::SK2;
                    }
                    else
                    {
                        _lLine = BOX::C::SK_NO;
                    }
                }
                else
                {
                    return JM_ERROR_GENERIC;
                }
                return JM_ERROR_SUCCESS;
            }

            size_t sendOneFrame(const guint8 *data, size_t count, gboolean needRecv)
            {
                guint8 sendBuff[256];
                size_t length;

                _shared->buffID = 0;

                if (!_box->newBatch(_shared->buffID))
                {
                    return 0;
                }

                length = pack(data, count, sendBuff);

                if (length <= 0)
                {
                    return 0;
                }

                if (needRecv)
                {
                    if (!_box->turnOverOneByOne() ||
                        !_box->sendOutData(sendBuff, length) ||
                        !_box->updateBuff(BOX::C::INC_DATA, _buffIDAddr) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BOX::C::REC_FR) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BOX::C::REC_LEN_1) ||
                        !_box->updateBuff(BOX::C::INC_DATA, _buffIDAddr) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BOX::C::REC_FR) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BOX::C::REC_LEN_1) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, FALSE))
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
                        !_box->updateBuff(BOX::C::INC_DATA, _buffIDAddr) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BOX::C::REC_FR) ||
                        !_box->turnOverOneByOne() ||
                        !_box->runReceive(BOX::C::REC_LEN_1) ||
                        !_box->updateBuff(BOX::C::INC_DATA, _buffIDAddr) ||
                        !_box->endBatch() ||
                        !_box->runBatch(&_shared->buffID, 1, FALSE))
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

            size_t sendOneFrame(const guint8 *data, 
                size_t count)
            {
                return sendOneFrame(data, count, FALSE);
            }

            size_t sendFrames(const guint8 *data, size_t count)
            {
                return sendOneFrame(data, count);
            }

            size_t readOneFrame(guint8 *data, gboolean isFinish)
            {
                size_t pos = 0;
                size_t len;
                guint8 temp[256];

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

                return unpack(temp, pos, data);
            }

            size_t readOneFrame(guint8 *data)
            {
                return readOneFrame(data, TRUE);
            }

            size_t readFrames(guint8 *data)
            {
                return readOneFrame(data);
            }

            gint32 setKeepLink(const guint8 *data, size_t count)
            {
                guint8 add1 = 0;
                guint8 add2 = 0;
                guint8 tempBuff[3];

                _buffIDAddr[0] = BOX::C::LINKBLOCK;
                if (_box->newBatch(BOX::C::LINKBLOCK))
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
                    !((add1 = _box->updateBuff(BOX::C::INC_DATA, _buffIDAddr)) == 0) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BOX::C::REC_FR) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BOX::C::REC_LEN_1) ||
                    !((add2 = _box->updateBuff(BOX::C::INC_DATA, _buffIDAddr)) == 0) ||
                    !_box->endBatch())
                {
                    return JM_ERROR_GENERIC;
                }

                tempBuff[0] = BOX::C::LINKBLOCK;
                tempBuff[1] = add1;
                tempBuff[2] = _buffIDAddr[1];

                if (!_box->getAbsAdd(BOX::C::LINKBLOCK, tempBuff[2]))
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->updateBuff(BOX::C::UPDATE_1BYTE, tempBuff))
                {
                    return JM_ERROR_GENERIC;
                }

                tempBuff[1] = add2;

                if (!_box->updateBuff(BOX::C::UPDATE_1BYTE, tempBuff))
                {
                    return JM_ERROR_GENERIC;
                }

                return JM_ERROR_SUCCESS;
            }
        private:
            BOX *_box;
            Shared *_shared;
            Default<BOX, JM::V1::KWP1281<BOX> > *_default;
            guint8 _buffIDAddr[2]; // 0 == buffID, 1 == buffAddr
            guint8 _kLine;
            guint8 _lLine;
        };
    }
}

#endif

