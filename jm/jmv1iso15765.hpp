#ifndef __JM_V1_ISO15765_HPP__
#define __JM_V1_ISO15765_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmcanbus.hpp"
#include "jmv1default.hpp"

namespace JM
{
    namespace V1
    {
        static const guint8 JM_V1_ISO15765_SJA_BTR_CODETABLE[30] =
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

        template<typename BOX>
        class ISO15765 : public JM::Canbus
        {
        public:
            static const guint8 SET_CANBEGIN = 0xF0;
            static const guint8 SET_CANBAUD = 0xF1;
            static const guint8 SET_CANACR = 0xF2;
            static const guint8 SET_CANAMR = 0xF3;
            static const guint8 SET_CANMODE = 0xF4;
            static const guint8 SET_CANCHAN = 0xF5;
            static const guint8 SET_CANPRIOR = 0xF6;
            static const guint8 SET_CANEND = 0xFA;
            static const guint8 SET_CANRESET = 0xFB;

            static const guint8 CAN_ACR_ACF1 = 0x20;
            static const guint8 CAN_ACR_ACF2 = 0x28;
            static const guint8 CAN_ACR_ACF3 = 0x30;
            static const guint8 CAN_ACR_ACF4 = 0x38;

            static const guint8 CAN_AMR_ACF1 = 0x24;
            static const guint8 CAN_AMR_ACF2 = 0x2C;
            static const guint8 CAN_AMR_ACF3 = 0x34;
            static const guint8 CAN_AMR_ACF4 = 0x3C;

            static const guint8 ACF1_FT1_PRIO = 0x01;
            static const guint8 ACF1_FT2_PRIO = 0x02;
            static const guint8 ACF2_FT1_PRIO = 0x04;
            static const guint8 ACF2_FT2_PRIO = 0x08;
            static const guint8 ACF3_FT1_PRIO = 0x10;
            static const guint8 ACF3_FT2_PRIO = 0x20;
            static const guint8 ACF4_FT1_PRIO = 0x40;
            static const guint8 ACF4_FT2_PRIO = 0x80;

            static const guint8 ACF1_FT1_CHAN = 0x01;
            static const guint8 ACF1_FT2_CHAN = 0x02;
            static const guint8 ACF2_FT1_CHAN = 0x04;
            static const guint8 ACF2_FT2_CHAN = 0x08;
            static const guint8 ACF3_FT1_CHAN = 0x10;
            static const guint8 ACF3_FT2_CHAN = 0x20;
            static const guint8 ACF4_FT1_CHAN = 0x40;
            static const guint8 ACF4_FT2_CHAN = 0x80;

            static const guint8 STD_FRAMEID_LENGTH = 0x02;
            static const guint8 EXT_FRAMEID_LENGTH = 0x04;
            static const guint8 SJA_OK = 0;
        public:
            ISO15765(BOX *box, Shared *shared)
                : _box(box)
                , _shared(shared)
                , _default(NULL)
            {
                _default = new JM::V1::Default<BOX, JM::V1::ISO15765<BOX> >(_box, _shared, this);
            }

            ~ISO15765()
            {
                delete _default;
            }

            void finishExecute(gboolean isFinish)
            {
                if (isFinish)
                {
                    _box->stopNow(TRUE);
                    _box->delBatch(_shared->buff_id);
                    _box->checkResult(JM_TIMER_TO_MS(200));
                }
            }

            gint32 setOptions(gint32 id, JMCanbusBaud baud,
                JMCanbusIDMode idMode,
                JMCanbusFilterMask mask,
                JMCanbusFrameType frame)
            {
                if (JM::Canbus::setOptions(id, baud, idMode, mask, frame))
                {
                    _targetID = (_targetID << 5) & 0x0000FFFF;
                    return JM_ERROR_SUCCESS;
                }

                return JM_ERROR_GENERIC;
            }

            gboolean prepare()
            {
                guint32 localID = 0;
                guint8 ctrlWord1;

                ctrlWord1 = C::RS_232 | C::BIT9_MARK | C::SEL_SL | C::UN_DB20;

                if (!_box->setCommCtrl(C::RZFC, C::SET_NULL) ||
                    !_box->setCommLine(C::SK5, C::SK7) ||
                    !_box->setCommLink(ctrlWord1, C::SET_NULL, C::SET_NULL) ||
                    !_box->setCommBaud(57600) ||
                    !_box->setCommTime(C::SETBYTETIME, 500) ||
                    !_box->setCommTime(C::SETWAITTIME, JM_TIMER_TO_MS(55)) ||
                    !_box->setCommTime(C::SETRECBBOUT, JM_TIMER_TO_MS(100)) ||
                    !_box->setCommTime(C::SETRECFROUT, JM_TIMER_TO_MS(200)) ||
                    !_box->setCommTime(C::SETLINKTIME, JM_TIMER_TO_MS(500)))
                {
                    return FALSE;
                }
                localID = GPOINTER_TO_UINT(g_ptr_array_index(_idArray, 0));

                g_usleep(JM_TIMER_TO_MS(100));
                if (!beginSet() ||
                    !setBaud(JM_CANBUS_B500K) ||
                    !setAcr(CAN_ACR_ACF1, JM_HIGH_BYTE(JM_HIGH_WORD(localID << 21)), JM_LOW_BYTE(JM_HIGH_WORD(localID << 21)), JM_HIGH_BYTE(JM_LOW_WORD(localID << 21)), JM_LOW_BYTE(JM_LOW_WORD(localID << 21))) ||
                    !setAmr(self, CAN_AMR_ACF1, 0x00, 0x1F, 0xFF, 0xFF) ||
                    !setMode(0x55) ||
                    !setPrior(0xFF) ||
                    !setChan(ACF1_FT1_CHAN) ||
                    !endSet())
                {
                    return FALSE;
                }
                return TRUE;
            }

            size_t sendOneFrame(const guint8 *data, size_t count, gboolean isFinish)
            {
                return _default->sendOneFrame(data, count, isFinish);
            }

            size_t sendOneFrame(const guint8 *data, size_t count)
            {
                return sendOneFrame(data, count, FALSE);
            }

            size_t sendFrames(const guint8 *data, size_t count)
            {
                if (count <= 0 || count > 0x0FFF)
                {
                    return 0;
                }

                if (count < 8)
                {
                    guint8 temp[8] = {0};

                    temp[0] = JM_LOW_BYTE(count);
                    memcpy(temp + 1, data, count);
                    return sendOneFrame(temp, count, TRUE);
                }
                else
                {
                    size_t frame_count = (count + 1) / 7;
                    size_t last_data = (count + 1) % 7;
                    size_t pos = 0;
                    guint8 seq = 0x21;
                    size_t frame_index = 0;

                    if (last_data != 0)
                        frame_count++;

                    for(; frame_index < frame_count; ++frame_index)
                    {
                        if (0 == frame_index)
                        {
                            guint8 temp[16];
                            temp[0] = (0x10 | JM_HIGH_BYTE(JM_LOW_WORD(count)));
                            temp[1] = JM_LOW_BYTE(JM_LOW_WORD(count));
                            memcpy(temp + 2, data + pos, 6);
                            pos += 6;
                            if (sendOneFrame(temp, 8, TRUE) != 8)
                            {
                                return 0;
                            }
                            if (readOneFrame(temp) <= 0)
                                return 0;
                        }
                        else if (frame_index == (frame_count - 1))
                        {
                            guint8 temp[8] = {0};
                            temp[0] = seq;

                            memcpy(temp + 1, data + pos, count - pos);
                            if (sendOneFrame(temp, count - pos + 1, TRUE) != 
                                (count - pos + 1))
                                return 0;
                            return count;
                        }
                        else
                        {
                            guint8 temp[8];
                            temp[0] = seq;
                            memcpy(temp + 1, data + pos, 7);
                            pos += 7;
                            if (sendOneFrame(temp, 8, FALSE) != 8)
                                return 0;

                            if (seq == 0x2F)
                                seq = 0x20;
                            else
                                seq++;
                        }
                    }
                }
                return 0;
            }

            size_t readOneFrame(guint8 *data)
            {
                return readOneFrame(data, TRUE);
            }

            size_t readFrames(guint8 *data)
            {
                guint8 firstFrame[16];
                size_t length;

                length = readOneFrame(firstFrame, FALSE);

                if (firstFrame[0] == 0x30)
                {
                    finishExecute(TRUE);
                    memcpy(data, firstFrame, length);
                    return length;
                }

                if ((firstFrame[0] & 0x10) == 0x10)
                {
                    // Multi-Frame
                    static size_t userDataCount;
                    static size_t resetFrameCount;
                    static size_t dataLength;
                    static size_t restDataCount;
                    static size_t i;
                    static size_t pos;
                    static guint8 restFrames[0xFFF];
                    static guint8 unpackedBuff[0xFF];

                    finishExecute(TRUE);

                    if (sendOneFrame(_flowControl, 8, 
                        TRUE) != 8)
                    {
                        return 0;
                    }

                    pos = 0;
                    userDataCount = ((firstFrame[0] & 0x0F) << 8) | 
                        (firstFrame[1] & 0xFF);
                    resetFrameCount = ((userDataCount - 6) / 7) + 
                        (((userDataCount - 6) % 7) ? 1 : 0);
                    memcpy(data, firstFrame + 2, length - 2);
                    pos += length - 2;

                    if (_idMode == JM_CANBUS_ID_MODE_STD)
                    {
                        dataLength = STD_FRAMEID_LENGTH + 8;
                    }
                    else
                    {
                        dataLength = EXT_FRAMEID_LENGTH + 8;
                    }

                    restDataCount = dataLength * resetFrameCount;

                    if (_box->readBytes(restFrames, restDataCount) != 
                        restDataCount)
                    {
                        finishExecute(TRUE);
                        return 0;
                    }

                    finishExecute(TRUE);
                    for (i = 0; i < resetFrameCount; i++)
                    {
                        length = unpack(restFrames + 
                            (i * dataLength), dataLength, unpackedBuff);
                        memcpy(data + pos, unpackedBuff, length);
                        pos += length;
                    }
                }
                else
                {
                    while (firstFrame[1] == 0x7F && firstFrame[3] == 0x78)
                    {
                        length = readOneFrame(firstFrame, 
                            FALSE);
                    }
                    finishExecute(TRUE);
                    memcpy(data, firstFrame + 1, length - 1);
                    return length;
                }
                return 0;
            }

            gint32 setKeepLink(const guint8 *data, size_t count)
            {
                static guint8 buff[256];
                static size_t length;

                length = pack(data, count, buff);

                return _default->setKeepLink(buff, length);
            }
        private:
            size_t sendCmd(const guint8 *data, size_t count)
            {
                _shared->buff_id = 0;
                if (_box->newBatch(_shared->buff_id))
                {
                    return 0;
                }

                if (!_box->sendOutData(data, count) ||
                    !_box->runReceive(C::RECEIVE) ||
                    !_box->endBatch() ||
                    !_box->runBatch(&_shared->buff_id, 1, FALSE))
                {
                    _box->delBatch(_shared->buff_id);
                    return 0;
                }

                g_usleep(count * _shared->req_byte_to_byte + 
                    _shared->req_wait_time);
                return count;
            }

            size_t readCmd(guint8 *buff)
            {
                size_t length;

                if (_box->readBytes(buff, 3) != 3)
                {
                    finishExecute(TRUE);
                    return 0;
                }

                length = buff[0] & 0x0F;

                switch(buff[0] & 0xC0)
                {
                case JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA:
                case JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_REMOTE:
                    length += STD_FRAMEID_LENGTH;
                    break;
                case (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA):
                case (JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE):
                    length += EXT_FRAMEID_LENGTH;
                    break;
                default:
                    finishExecute(TRUE);
                    return 0;
                }

                length -= 2;
                if (length > 0)
                {
                    if (_box->readBytes(buff + 3, length) != length)
                    {
                        finishExecute(TRUE);
                        return length + 3;
                    }
                    else
                    {
                        return 0;
                    }
                }
                finishExecute(TRUE);
                return length + 3;
            }

            gboolean doCmd(guint8 cmd, guint8 inf)
            {
                guint8 buff[256] = {0x20 | 2, 0x55, 0xAA, cmd, inf};
                size_t length;

                if (sendCmd(buff, 5) != 5)
                {
                    return FALSE;
                }
                length = readCmd(buff);

                if (length <= 0)
                {
                    return FALSE;
                }

                if (buff[4] != SJA_OK)
                {
                    return FALSE;
                }

                return TRUE;
            }

            inline gboolean beginSet()
            {
                return doCmd(SET_CANBEGIN, 0);
            }

            inline gboolean setMode(guint8 mode)
            {
                return doCmd(SET_CANMODE, mode);
            }

            inline gboolean setPrior(guint8 prior)
            {
                return doCmd(SET_CANPRIOR, prior);
            }

            inline gboolean setChan(guint8 chan)
            {
                return doCmd(SET_CANCHAN, chan);
            }

            inline gboolean endSet()
            {
                return doCmd(SET_CANEND, 0);
            }

            gboolean setBaud(JMCanbusBaud baud)
            {
                guint8 buff[256] = {0x20 | 5, 0x55, 0xAA, SET_CANBAUD};

                switch(baud)
                {
                case JM_CANBUS_B1000K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[28];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[29];
                    break;
                case JM_CANBUS_B800K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[26];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[27];
                    break;
                case JM_CANBUS_B666K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[24];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[25];
                    break;
                case JM_CANBUS_B500K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[22];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[23];
                    break;
                case JM_CANBUS_B400K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[20];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[21];
                    break;
                case JM_CANBUS_B250K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[18];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[19];
                    break;
                case JM_CANBUS_B200K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[16];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[17];
                    break;
                case JM_CANBUS_B125K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[14];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[15];
                    break;
                case JM_CANBUS_B100K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[12];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[13];
                    break;
                case JM_CANBUS_B80K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[10];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[11];
                    break;
                case JM_CANBUS_B50K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[8];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[9];
                    break;
                case JM_CANBUS_B40K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[6];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[7];
                    break;
                case JM_CANBUS_B20K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[4];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[5];
                    break;
                case JM_CANBUS_B10K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[2];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[3];
                    break;
                case JM_CANBUS_B5K:
                    buff[4] = JM_V1_ISO15765_SJA_BTR_CODETABLE[0];
                    buff[5] = JM_V1_ISO15765_SJA_BTR_CODETABLE[1];
                    break;
                default:
                    return FALSE;
                    break;
                }

                if (sendCmd(buff, 6) != 6)
                {
                    return FALSE;
                }

                if (!readCmd(buff) <= 0)
                {
                    return FALSE;
                }

                if (buff[4] != SJA_OK)
                {
                    return FALSE;
                }

                return TRUE;
            }

            gboolean setAcr(guint8 acr, guint8 acr0, guint8 acr1, 
                guint8 acr2, guint8 acr3)
            {
                guint8 buff[256] = {0x20 | 6, 0x55, 0xAA, SET_CANACR, 
                    acr, acr0, acr1, acr2, acr3};

                if (sendCmd(buff, 9) != 9)
                {
                    return FALSE;
                }

                if (readCmd(buff) <= 0)
                {
                    return FALSE;
                }

                if (buff[4] != SJA_OK)
                {
                    return FALSE;
                }

                return TRUE;
            }

            static gboolean setAmr(guint8 amr, guint8 amr0, guint8 amr1, 
                guint8 amr2, guint8 amr3)
            {
                guint8 buff[256] = {0x20 | 6, 0x55, 0xAA, SET_CANAMR, 
                    amr, amr0, amr1, amr2, amr3};

                if (sendCmd(buff, 9) != 9)
                {
                    return FALSE;
                }

                if (readCmd(buff) <= 0)
                {
                    return FALSE;
                }

                if (buff[4] != SJA_OK)
                {
                    return FALSE;
                }

                return TRUE;
            }

            size_t readOneFrame(guint8 *buff, gboolean isFinish)
            {
                static size_t length;
                static size_t mode;

                if (_box->readBytes(buff, 3) != 3)
                {
                    finishExecute(TRUE);
                    return 0;
                }

                length = buff[0] & 0x0F;
                mode = buff[0] & (JM_CANBUS_ID_MODE_EXT | 
                    JM_CANBUS_FRAME_TYPE_REMOTE);

                switch(mode)
                {
                case JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_DATA:
                case JM_CANBUS_ID_MODE_STD | JM_CANBUS_FRAME_TYPE_REMOTE:
                    length += STD_FRAMEID_LENGTH;
                    break;
                case JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_DATA:
                case JM_CANBUS_ID_MODE_EXT | JM_CANBUS_FRAME_TYPE_REMOTE:
                    length += EXT_FRAMEID_LENGTH;
                    break;
                default:
                    finishExecute(TRUE);
                    return 0;
                    break;
                }

                length -= 2;
                if (length <= 0)
                {
                    finishExecute(TRUE);
                    return 0;
                }
                else
                {
                    if (_box->readBytes(buff + 3, length) != length)
                    {
                        finishExecute(TRUE);
                        return 0;
                    }
                    finishExecute(TRUE);
                    return length + 3;
                }
                return 0;
            }

        private:
            BOX *_box;
            Shared *_shared;
            Default<BOX, JM::V1::ISO15765<BOX> > *_default;
        };
    }
}

#endif