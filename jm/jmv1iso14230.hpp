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
        template<typename BOX>
        class ISO14230 : public JM::KWP2000
        {
        public:
            ISO14230(BOX *box, Shared *shared)
                : _box(box)
                , _shared(shared)
                , _default(NULL)
                , _lLine(FALSE)
                , _sendLine(0)
                , _recvLine(0)
            {
                _default = new JM::V1::Default<BOX, JM::V1::ISO14230<BOX> >(_box, _shared, this);
            }

            ~ISO14230()
            {
                delete _default;
            }
            gint32 fastInit(const guint8 *data, size_t count)
            {
                guint8 valueOpen = 0;
                guint8 packEnter[256];
                size_t length;

                if (_lLine)
                {
                    valueOpen = BOX::C::PWC | BOX::C::RZFC | BOX::C::CK;
                }
                else
                {
                    valueOpen = BOX::C::PWC | BOX::C::RZFC | BOX::C::CK;
                }

                if (!_box->setCommCtrl(valueOpen, BOX::C::SET_NULL) ||
                    !_box->setCommLine(_sendLine, _recvLine) ||
                    !_box->setCommLink(BOX::C::RS_232 | BOX::C::BIT9_MARK | BOX::C::SEL_SL | BOX::C::UN_DB20, BOX::C::SET_NULL, BOX::C::SET_NULL) ||
                    !_box->setCommBaud(10416) ||
                    !_box->setCommTime(BOX::C::SETBYTETIME, JM_TIMER_TO_MS(5)) ||
                    !_box->setCommTime(BOX::C::SETWAITTIME, JM_TIMER_TO_MS(0)) ||
                    !_box->setCommTime(BOX::C::SETRECBBOUT, JM_TIMER_TO_MS(400)) ||
                    !_box->setCommTime(BOX::C::SETRECFROUT, JM_TIMER_TO_MS(500)) ||
                    !_box->setCommTime(BOX::C::SETLINKTIME, JM_TIMER_TO_MS(500)))
                {
                    return JM_ERROR_GENERIC;
                }

                g_usleep(JM_TIMER_TO_SEC(1));
                _shared->buff_id = 0;

                if (!_box->newBatch(_shared->buff_id))
                {
                    return JM_ERROR_GENERIC;
                }

                length = pack(data, count, packEnter);

                if (!_box->setLineLevel(BOX::C::COMS, BOX::C::SET_NULL) ||
                    !_box->commboxDelay(JM_TIMER_TO_MS(25)) ||
                    !_box->setLineLevel(BOX::C::SET_NULL, BOX::C::COMS) ||
                    !_box->commboxDelay(JM_TIMER_TO_MS(25)) ||
                    !_box->sendOutData(packEnter, length) ||
                    !_box->runReceive(BOX::C::REC_FR) ||
                    !_box->endBatch())
                {
                    _box->delBatch(_shared->buff_id);
                    return JM_ERROR_GENERIC;
                }

                if (!_box->runBatch(&(_shared->buff_id), 1, FALSE))
                    return JM_ERROR_GENERIC;

                length = readOneFrame(packEnter);
                if (length <= 0)
                    return JM_ERROR_GENERIC;

                _box->setCommTime(BOX::C::SETWAITTIME, JM_TIMER_TO_MS(55));
                return JM_ERROR_SUCCESS;
            }

            void finishExecute(gboolean isFinish)
            {
                if (isFinish)
                {
                    _box->stopNow(TRUE);
                    _box->delBatch(_shared->buff_id);
                    _box->checkResult(JM_TIMER_TO_MS(500));
                }
            }

            gint32 addrInit(guint8 addrCode)
            {
                guint8 temp[3];
                if (!_box->setCommCtrl(BOX::C::PWC | BOX::C::REFC |
                    BOX::C::RZFC | BOX::C::CK, BOX::C::SET_NULL) ||
                    !_box->setCommLink(BOX::C::RS_232 | BOX::C::BIT9_MARK |
                    BOX::C::SEL_SL | BOX::C::SET_DB20, BOX::C::SET_NULL,
                    BOX::C::INVERTBYTE) ||
                    !_box->setCommBaud(5) ||
                    !_box->setCommTime(BOX::C::SETBYTETIME, JM_TIMER_TO_MS(5)) ||
                    !_box->setCommTime(BOX::C::SETWAITTIME, JM_TIMER_TO_MS(12)) ||
                    !_box->setCommTime(BOX::C::SETRECBBOUT, JM_TIMER_TO_MS(400)) ||
                    !_box->setCommTime(BOX::C::SETRECFROUT, JM_TIMER_TO_MS(500)) ||
                    !_box->setCommTime(BOX::C::SETLINKTIME, JM_TIMER_TO_MS(500)))
                {
                    return JM_ERROR_GENERIC;
                }

                g_usleep(JM_TIMER_TO_SEC(1));

                _shared->buff_id = 0;

                if (!_box->newBatch(_shared->buff_id))
                {
                    return JM_ERROR_GENERIC;
                }

                if (!_box->sendOutData(&addrCode, 1) ||
                    !_box->setCommLine((_recvLine == BOX::C::RK_NO) ? _sendLine : BOX::C::SK_NO, _recvLine) ||
                    !_box->runReceive(BOX::C::SET55_BAUD) ||
                    !_box->runReceive(BOX::C::REC_LEN_1) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BOX::C::REC_LEN_1) ||
                    !_box->turnOverOneByOne() ||
                    !_box->runReceive(BOX::C::REC_LEN_1) ||
                    !_box->endBatch())
                {
                    _box->delBatch(_shared->buff_id);
                    return JM_ERROR_GENERIC;
                }

                if (!_box->runBatch(&(_shared->buff_id), 1, FALSE) ||
                    (_box->readData(temp, 0, JM_TIMER_TO_SEC(3)) <= 0) ||
                    !_box->checkResult(JM_TIMER_TO_SEC(5)))
                {
                    _box->delBatch(_shared->buff_id);
                    return JM_ERROR_GENERIC;
                }

                if (!_box->delBatch(_shared->buff_id) ||
                    !_box->setCommTime(BOX::C::SETWAITTIME, 
                    JM_TIMER_TO_MS(55)))
                {
                    return JM_ERROR_GENERIC;
                }

                if (temp[2] != 0)
                    return JM_ERROR_GENERIC;

                return JM_ERROR_SUCCESS;
            }

            gint32 setLines(gint32 comLine, gboolean lLine)
            {
                // According the connector to determine the send and receive line
                if (_shared->connector == JM_CN_OBDII_16)
                {
                    switch (comLine)
                    {
                    case 7:
                        _sendLine = BOX::C::SK1;
                        _recvLine = BOX::C::RK1;
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

            size_t sendOneFrame(const guint8 *data, size_t count)
            {
                return sendOneFrame(data, count, FALSE);
            }

            size_t sendFrames(const guint8 *data, size_t count)
            {
                return sendOneFrame(data, count);
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
                guint8 buff[256];
                size_t length;
                _mode = _linkMode;
                length = pack(data, count, buff);
                _mode = _msgMode;
                return _default->setKeepLink(buff, length);
            }
        private:
            size_t readOneFrame(guint8 *data, gboolean isFinish)
            {
                static guint8 temp[3];
                static guint8 result[256];
                size_t frameLength = 0;
                size_t length;
                guint8 checksum;
                size_t i;

                length = _box->readBytes(temp, 3);

                if (length <= 0)
                {
                    finishExecute(isFinish);
                    return 0;
                }

                if (temp[1] == _sourceAddress)
                {
                    if (temp[0] == 0x80)
                    {
                        guint8 b;
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
                        memcpy(result, temp, 3);
                        frameLength += 3;
                        memcpy(result + frameLength, &b, 1);
                        frameLength++;
                        length = _box->readBytes(result + KWP80_HEADER_LENGTH, 
                            length + KWP_CHECKSUM_LENGTH);
                        frameLength += length;
                    }
                    else
                    {
                        length = temp[0] - 0x80;
                        if (length <= 0)
                        {
                            return 0;
                        }

                        memcpy(result, temp, 3);
                        frameLength += 3;
                        length = _box->readBytes(result + 3, 
                            length + KWP_CHECKSUM_LENGTH);
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
                        memcpy(result, temp, 3);
                        frameLength += 3;
                        length = _box->readBytes(result + 3, length);
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
                        memcpy(result, temp, 3);
                        frameLength += 3;
                        length = _box->readBytes(result + 3, 
                            length - KWP_CHECKSUM_LENGTH);
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

                return unpack(result, frameLength, data);
            }

            gint32 sendOneFrame(const guint8 *data, size_t count, gboolean isFinish)
            {
                return _default->sendOneFrame(data, count, isFinish);
            }
        private:
            BOX *_box;
            Shared *_shared;
            Default<BOX, JM::V1::ISO14230<BOX> > *_default;
            gboolean _lLine;
            guint8 _sendLine;
            guint8 _recvLine;
        };
    }
}

#endif