#ifndef __JM_V1_BOX_C168_HPP__
#define __JM_V1_BOX_C168_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include "jmutils.h"
#include "jmv1box.hpp"
#include "jmserialport.hpp"
#include "jmtimer.h"

namespace JM
{
    namespace V1
    {
        namespace C168
        {
            class Box : public V1::Box<C168::CONSTANT>
            {
            public:
                Box(V1::Shared *shared)
                    : V1::Box<C168::CONSTANT>(shared)
                    , _buffUsedNum(0)
                    , _timeUnit(0)
                    , _timeBaseDB(0)
                    , _timeExternDB(0)
                    , _cmdBuffLen(0)
                    , _buffID(0)
                    , _headPassword(0)
                {
                    memset(_buffAdd, 0, C::MAXIM_BLOCK + 2);
                    memset(_buffUsed, 0, C::MAXIM_BLOCK);
                    memset(_version, 0, C::VERSIONLEN);
                    memset(_boxID, 0, C::COMMBOXIDLEN);
                    memset(_port, 0, C::COMMBOXPORTNUM);
                    memset(_cmdTemp, 0, sizeof(_cmdTemp));
                }
                gboolean openComm()
                {
                    if (jm_commbox_port_get_type() == JM_COMMBOX_PORT_SERIAL_PORT)
                    {
                        JMStringArray *vec = NULL;
                        JM::SerialPort *port = (JM::SerialPort*)jm_commbox_port_get_pointer();
                        size_t vec_length = 0;
                        size_t i;

                        if (_port == NULL)
                            return FALSE;

                        vec = JM::SerialPort::getSystemPorts();
                        vec_length = jm_string_array_size(vec);

                        for (i = 0; i < vec_length; i++)
                        {
                            port->setPortName(jm_string_array_get(vec, i));
                            port->setBaudrate(9600);
                            port->setParity(JM_SP_PAR_NONE);
                            port->setStopbits(JM_SP_SB_ONE);
                            port->setFlowControl(JM_SP_FC_NONE);
                            port->setDatabits(8);
                            if (port->open() == JM_ERROR_SUCCESS)
                            {
                                port->setDtr(TRUE);
                                if (openBox())
                                    return TRUE;
                            }
                            port->close();
                        }
                    }
                    return FALSE;
                }

                gboolean closeComm()
                {
                    stopNow(TRUE);
                    doSet(C::RESET, 0, NULL);
                    setRF(C::RESET_RF, 0);
                    if  (jm_commbox_port_get_type() == JM_COMMBOX_PORT_SERIAL_PORT)
                    {
                        ((JM::SerialPort*)jm_commbox_port_get_pointer())->close();
                    }
                    return TRUE;
                }

                gboolean delBatch(guint8 buffID)
                {
                    if (buffID > C::MAXIM_BLOCK)
                    {
                        // 数据块不存在
                        _shared->lastError = C::NODEFINE_BUFF;
                        return FALSE;
                    }

                    if (_buffID == buffID)
                    {
                        _buffID = C::NULLADD;
                        return TRUE;
                    }

                    if (_buffAdd[buffID] == C::NULLADD)
                    {
                        // 数据块标识登记是否有申请?
                        _shared->lastError = C::NOUSED_BUFF;
                        return FALSE;
                    }

                    if (buffID == C::LINKBLOCK)
                    {
                        _buffAdd[C::LINKBLOCK] = _cmdBuffLen;
                    }
                    else
                    {
                        size_t i = 0;
                        guint8 data[3];
                        guint8 deleteBuffLen;

                        for (; i < _buffUsedNum; i++)
                        {
                            if (_buffUsed[i] == buffID)
                            {
                                break;
                            }
                        }
                        data[0] = _buffAdd[buffID];
                        if (i < _buffUsedNum - 1)
                        {
                            data[1] = _buffAdd[_buffUsed[i + 1]];
                            data[2] = _buffAdd[C::SWAPBLOCK] - data[1];

                            if (!doSet(C::COPY_DATA - C::COPY_DATA % 4, 3, data))
                            {
                                return FALSE;
                            }
                        }
                        else
                        {
                            data[1] = _buffAdd[C::SWAPBLOCK];
                        }

                        deleteBuffLen = data[1] - data[0];
                        for (i = i + 1; i < _buffUsedNum; i++)
                        {
                            _buffUsed[i - 1] = _buffUsed[i];
                            _buffAdd[_buffUsed[i]] = _buffAdd[_buffUsed[i]] - deleteBuffLen;
                        }
                        _buffUsedNum--;
                        _buffAdd[C::SWAPBLOCK] = _buffAdd[C::SWAPBLOCK] - deleteBuffLen;
                        _buffAdd[_buffID] = C::NULLADD;
                    }
                    return TRUE;
                }

                gboolean newBatch(guint8 buffID)
                {
                    if (buffID > C::MAXIM_BLOCK)
                    {
                        _shared->lastError = C::NODEFINE_BUFF;
                        return FALSE;
                    }
                    if (_buffID != C::NULLADD)
                    {
                        _shared->lastError = C::APPLICATION_NOW;
                        return FALSE;
                    }

                    if ((_buffAdd[buffID] != C::NULLADD && 
                        (buffID != C::LINKBLOCK) && 
                        !delBatch(buffID)))
                    {
                        return FALSE;;
                    }

                    _cmdTemp[0] = C::WR_DATA;
                    _cmdTemp[1] = 0x01;
                    if (_buffID == C::LINKBLOCK)
                    {
                        _cmdTemp[2] = 0xFF;
                        _buffAdd[C::LINKBLOCK] = _cmdBuffLen;
                    }
                    else
                    {
                        _cmdTemp[2] = _buffAdd[C::SWAPBLOCK];
                    }

                    if (bufferSize() <= 1)
                    {
                        _shared->lastError = C::BUFFFLOW;
                        return FALSE;
                    }
                    _cmdTemp[3] = C::WR_DATA + 0x01 + _cmdTemp[2];
                    _cmdTemp[0] += _headPassword;
                    _buffID = buffID;
                    _shared->isDoNow = FALSE;
                    return TRUE;
                }

                gboolean addToBuff(guint8 commandWord, size_t count, const guint8 *data)
                {
                    size_t i;
                    guint8 checksum;

                    checksum = _cmdTemp[_cmdTemp[1] + 2];

                    _shared->nextAddress = _cmdTemp[1] + count + 1;

                    if (_buffID == C::NULLADD)
                    {
                        // 数据块标识登记是否有申请?
                        _shared->lastError = C::NOAPPLICATBUFF;
                        _shared->isDoNow = TRUE;
                        return FALSE;
                    }

                    if (bufferSize() < _shared->nextAddress)
                    {
                        // 检查是否有足够的空间存储?
                        _shared->lastError = C::BUFFFLOW;
                        _shared->isDoNow = TRUE;
                        return FALSE;
                    }

                    if (commandWord < C::RESET && 
                        commandWord != C::CLR_LINK &&
                        commandWord != C::DO_BAT_00 && 
                        commandWord != C::D0_BAT &&
                        commandWord != C::D0_BAT_FOR && 
                        commandWord != C::WR_DATA)
                    {
                        // 是否为缓冲区命令?
                        if ((count < C::CMD_DATALEN) ||
                            (commandWord == C::SEND_DATA && 
                            count < C::SEND_LEN))
                        {
                            // 是否合法命令?
                            if (commandWord == C::SEND_DATA && 
                                getBoxVer() > 0x400)
                            {
                                // 增加发送长命令
                                _cmdTemp[_cmdTemp[1] + 2] = C::SEND_CMD;
                                checksum += C::SEND_CMD;
                                _cmdTemp[1]++;
                                _cmdTemp[_cmdTemp[1] + 2] = commandWord + count;
                                if (count > 0)
                                {
                                    _cmdTemp[_cmdTemp[1] + 2]--;
                                }
                                checksum += _cmdTemp[_cmdTemp[1] + 2];
                                _cmdTemp[1]++;
                                for (i = 0; i < count; i++, _cmdTemp[1]++)
                                {
                                    _cmdTemp[_cmdTemp[1] + 2] = data[i];
                                    checksum += data[i];
                                }
                                _cmdTemp[_cmdTemp[1] + 2] = checksum + count + 2;
                                _shared->nextAddress++;
                            }
                            else
                            {
                                _cmdTemp[_cmdTemp[1] + 2] = commandWord + count;
                                if (count > 0)
                                {
                                    _cmdTemp[_cmdTemp[1] + 2]--;
                                }
                                checksum += _cmdTemp[_cmdTemp[1] + 2];
                                _cmdTemp[1]++;
                                for (i = 0; i < count; i++, _cmdTemp[1]++)
                                {
                                    _cmdTemp[_cmdTemp[1] + 2] = data[i];
                                    checksum += data[i];
                                }
                                _cmdTemp[_cmdTemp[1] + 2] = checksum + count + 1;
                                _shared->nextAddress++; // Rocky Add
                            }
                            return TRUE;
                        }
                        _shared->lastError = C::ILLIGICAL_LEN;
                        _shared->isDoNow = TRUE;
                        return FALSE;
                    }
                    _shared->lastError = C::UNBUFF_CMD;
                    _shared->isDoNow = TRUE;
                    return FALSE;
                }

                gboolean endBatch()
                {
                    guint32 times = C::REPLAYTIMES;

                    _shared->isDoNow = TRUE;

                    if (_buffID == C::NULLADD)
                    {
                        // 数据块标识登记是否有申请?
                        _shared->lastError = C::NOAPPLICATBUFF;
                        return FALSE;
                    }

                    if (_cmdTemp[1] == 0x01)
                    {
                        _buffID = C::NULLADD;
                        _shared->lastError = C::NOADDDATA;
                        return FALSE;
                    }

                    while (times--)
                    {
                        if (times == 0)
                        {
                            _buffID = C::NULLADD;
                            return FALSE;
                        }
                        if (!checkIdle() || 
                            (jm_commbox_port_write(_cmdTemp, _cmdTemp[1] + 3) != (_cmdTemp[1] + 3)))
                        {
                            continue;
                        }
                        else if (sendOK(JM_TIMER_TO_MS(20) * (_cmdTemp[1] + 10)))
                        {
                            break;
                        }
                        if (!stopNow(TRUE))
                        {
                            _buffID = C::NULLADD;
                            return FALSE;
                        }
                    }

                    if (_buffID == C::LINKBLOCK)
                    {
                        _buffAdd[C::LINKBLOCK] = _cmdBuffLen - _cmdTemp[1];
                    }
                    else
                    {
                        _buffAdd[_buffID] = _buffAdd[C::SWAPBLOCK];
                        _buffUsed[_buffUsedNum] = _buffID;
                        _buffUsedNum++;
                        _buffAdd[C::SWAPBLOCK] = _buffAdd[C::SWAPBLOCK] + _cmdTemp[1];
                    }
                    _buffID = C::NULLADD;
                    return TRUE;
                }

                gboolean sendToBox(guint8 commandWord, size_t count, const guint8 *buff)
                {
                    if (_shared->isDoNow)
                    {
                        return doSet(commandWord, count, buff);
                    }
                    else
                    {
                        return addToBuff(commandWord, count, buff);
                    }
                }

                gboolean setLineLevel(guint8 valueLow, guint8 valueHigh)
                {
                    // 只有一个字节的数据，设定端口1
                    _port[1] &= ~valueLow;
                    _port[1] |= valueHigh;
                    return sendToBox(C::SETPORT1, 1, _port + 1);
                }

                gboolean setCommCtrl(guint8 valueOpen, guint8 valueClose)
                {
                    // 只有一个字节的数据，设定端口2
                    _port[2] &= ~valueOpen;
                    _port[2] |= valueClose;
                    return sendToBox(C::SETPORT2, 1, _port + 2);
                }

                gboolean setCommLine(guint8 sendLine, guint8 recvLine)
                {
                    // 只有一个字节的数据，设定端口0
                    if (sendLine > 7)
                    {
                        sendLine = 0x0F;
                    }
                    if (recvLine > 7)
                    {
                        recvLine = 0x0F;
                    }
                    _port[0] = sendLine + (recvLine << 4);
                    return sendToBox(C::SETPORT0, 1, _port);
                }

                gboolean turnOverOneByOne()
                {
                    // 将原有的接受一个发送一个的标志翻转
                    return sendToBox(C::SET_ONEBYONE, 0, NULL);
                }

                gboolean setEchoData(guint8 *echoBuff, size_t echoLen)
                {
                    if (echoBuff == NULL || echoLen == 0 || echoLen > 4)
                    {
                        _shared->lastError = C::ILLIGICAL_LEN;
                        return FALSE;
                    }
                    if (_shared->isDoNow)
                    {
                        guint8 receiveBuff[6];
                        size_t i;
                        if (!commboxDo(C::ECHO, echoLen, echoBuff) || 
                            (readData(receiveBuff, echoLen, JM_TIMER_TO_MS(100)) != echoLen))
                        {
                            return FALSE;
                        }
                        for (i = 0; i < echoLen; i++)
                        {
                            if (receiveBuff[i] != echoBuff[i])
                            {
                                _shared->lastError = C::CHECKSUM_ERROR;
                                return FALSE;
                            }
                        }
                        return checkResult(JM_TIMER_TO_MS(100));
                    }
                    return addToBuff(C::ECHO, echoLen, echoBuff);
                }

                gboolean keepLink(gboolean isRunLink)
                {
                    if (isRunLink)
                    {
                        return sendToBox(C::RUNLINK, 0, NULL);
                    }
                    return sendToBox(C::STOPLINK, 0, NULL);
                }

                gboolean setCommLink(guint8 ctrlWord1, guint8 ctrlWord2, guint8 ctrlWord3)
                {
                    guint8 ctrlWord[3]; // 通讯控制字3
                    guint8 modeControl = ctrlWord1 & 0xE0;
                    size_t length = 3;

                    ctrlWord[0] = ctrlWord1;

                    if ((ctrlWord1 & 0x04) != 0)
                    {
                        _shared->isDB20 = TRUE;
                    }
                    else
                    {
                        _shared->isDB20 = FALSE;
                    }

                    if (modeControl == C::SET_VPW || 
                        modeControl == C::SET_PWM)
                    {
                        return sendToBox(C::SETTING, 1, ctrlWord);
                    }

                    ctrlWord[1] = ctrlWord2;
                    ctrlWord[2] = ctrlWord3;
                    if (ctrlWord3 == 0)
                    {
                        length--;
                        if (ctrlWord2 == 0)
                        {
                            length--;
                        }
                    }
                    if (modeControl == C::EXRS_232 && length < 2)
                    {
                        _shared->lastError = C::UNSET_EXRSBIT;
                        return FALSE;
                    }

                    return sendToBox(C::SETTING, length, ctrlWord);
                }

                gboolean setCommBaud(gdouble baud)
                {
                    guint8 baudTime[2];
                    gdouble instructNum;

                    instructNum = ((1000000.0 / (_timeUnit)) * 1000000) / baud;

                    if (_shared->isDB20)
                    {
                        instructNum /= 20;
                    }

                    instructNum += 0.5;
                    if (instructNum > 65535 || instructNum < 10)
                    {
                        _shared->lastError = C::COMMBAUD_OUT;
                        return FALSE;
                    }
                    baudTime[0] = JM_HIGH_BYTE(instructNum);
                    baudTime[1] = JM_LOW_BYTE(instructNum);
                    if (baudTime[0] == 0)
                    {
                        return sendToBox(C::SETBAUD, 1, baudTime + 1);
                    }
                    else
                    {
                        return sendToBox(C::SETBAUD, 2, baudTime);
                    }
                }

                void getLinkTime(guint8 type, guint32 time)
                {
                    if (type == C::SETBYTETIME)
                    {
                        _shared->reqByteToByte = time;
                    }
                    else if (type == C::SETWAITTIME)
                    {
                        _shared->reqWaitTime = time;
                    }
                    else if (type == C::SETRECBBOUT)
                    {
                        _shared->resByteToByte = time;
                    }
                    else if (type == C::SETRECFROUT)
                    {
                        _shared->resWaitTime = time;
                    }
                }

                gboolean setCommTime(guint8 type, guint32 time)
                {
                    guint8 timeBuff[2];

                    getLinkTime(type, time);

                    if (type == C::SETVPWSTART || 
                        type == C::SETVPWRECS)
                    {
                        if (C::SETVPWRECS == type)
                        {
                            time = (time * 2) / 3;
                        }
                        type = type + (C::SETBYTETIME & 0xF0);
                        time = time / ((_timeUnit / 1000000.0));
                    }
                    else
                    {
                        time = ((time / _timeBaseDB) / (_timeUnit / 1000000.0));
                    }

                    if (time > 65535)
                    {
                        _shared->lastError = C::COMMTIME_OUT;
                        return FALSE;
                    }

                    if (type == C::SETBYTETIME ||
						type == C::SETWAITTIME ||
						type == C::SETRECBBOUT ||
						type == C::SETRECFROUT ||
						type == C::SETLINKTIME)
                    {
                        timeBuff[0] = JM_HIGH_BYTE(time);
                        timeBuff[1] = JM_LOW_BYTE(time);
                        if (timeBuff[0] == 0)
                        {
                            return sendToBox(type, 1, timeBuff + 1);
                        }
                        return sendToBox(type, 2, timeBuff);
                    }
                    _shared->lastError = C::UNDEFINE_CMD;
                    return FALSE;
                }

                gboolean runReceive(guint8 type)
                {
                    if (type == C::GET_PORT1)
                    {
                        _shared->isDB20 = FALSE;
                    }

                    if (type == C::GET_PORT1 || 
                        type == C::SET55_BAUD || 
                        (type >= C::REC_FR && type <= C::RECEIVE))
                    {
                        if (_shared->isDoNow)
                        {
                            return commboxDo(type, 0, NULL);
                        }
                        return addToBuff(type, 0, NULL);
                    }
                    _shared->lastError = C::UNDEFINE_CMD;
                    return FALSE;
                }

                guint8 getAbsAdd(guint8 _buffID, guint8 add)
                {
                    size_t length = 0;
                    guint8 start_add = 0;

                    if (_buffID != _buffID)
                    {
                        if (_buffAdd[_buffID] == C::NULLADD)
                        {
                            _shared->lastError = C::NOUSED_BUFF;
                            return 0;
                        }

                        if (_buffID == C::LINKBLOCK)
                        {
                            length = _cmdBuffLen - _buffAdd[C::LINKBLOCK];
                        }
                        else
                        {
                            size_t i;
                            for (i = 0; i < _buffUsedNum; i++)
                            {
                                if (_buffUsed[i] == _buffID)
                                {
                                    break;
                                }
                            }
                            if (i == (_buffUsedNum - 1))
                            {
                                length = _buffAdd[C::SWAPBLOCK] - _buffAdd[_buffID];
                            }
                            else
                            {
                                length = _buffAdd[_buffID + 1] - _buffAdd[_buffID];
                            }
                        }
                        start_add = _buffAdd[_buffID];
                    }
                    else
                    {
                        length = _buffAdd[C::LINKBLOCK] - _buffAdd[C::SWAPBLOCK];
                        start_add = _buffAdd[C::SWAPBLOCK];
                    }

                    if (add < length)
                    {
                        return add + start_add;
                    }

                    _shared->lastError = C::OUTADDINBUFF;
                    return 0;
                }

                gboolean updateBuff(guint8 type, guint8 *buffer)
                {
                    guint8 cmdTemp[4];
                    guint8 ret;

                    _shared->lastError = 0;
                    ret = getAbsAdd(buffer[0], buffer[1]);
                    if (ret == 0)
                    {
                        return FALSE;
                    }
                    cmdTemp[0] = ret;

                    if ((type == C::INVERT_DATA) || 
                        (type == C::DEC_DATA) || 
                        (type == C::INC_DATA))
                    {
                    }
                    else if ((type == C::UPDATE_1BYTE) || 
                        (type == C::SUB_BYTE))
                    {
                        cmdTemp[1] = buffer[2];
                    }
                    else if (type == C::INC_2DATA)
                    {
                        ret = getAbsAdd(buffer[2], buffer[3]);
                        if (ret == 0)
                        {
                            return FALSE;
                        }
                        cmdTemp[1] = ret;
                    }
                    else if ((type == C::COPY_DATA) || 
                        (type == C::ADD_1BYTE))
                    {
                        ret = getAbsAdd(buffer[2], buffer[3]);
                        if (ret == 0)
                        {
                            return FALSE;
                        }
                        cmdTemp[1] = ret;
                        cmdTemp[2] = buffer[4];
                    }
                    else if ((type == C::UPDATE_2BYTE) || 
                        (type == C::ADD_2BYTE))
                    {
                        ret = getAbsAdd(buffer[3], buffer[4]);
                        if (ret == 0)
                        {
                            return FALSE;
                        }
                        cmdTemp[1] = buffer[2];
                        cmdTemp[2] = ret;
                        cmdTemp[3] = buffer[5];

                    }
                    else if ((type == C::ADD_DATA) || 
                        (type == C::SUB_DATA))
                    {
                        ret = getAbsAdd(buffer[2], buffer[3]);
                        if (ret == 0)
                        {
                            return FALSE;
                        }
                        cmdTemp[1] = ret;
                        ret = getAbsAdd(buffer[4], buffer[5]);
                        if (ret == 0)
                        {
                            return FALSE;
                        }
                        cmdTemp[2] = ret;

                    }
                    else
                    {
                        _shared->lastError = C::UNDEFINE_CMD;
                        return FALSE;

                    }
                    return sendToBox(type - type % 4, type % 4 + 1, cmdTemp);
                }

                gboolean commboxDelay(guint32 time)
                {
                    guint8 timeBuff[2];
                    guint8 delayWord = C::DELAYSHORT;

                    time = time / (_timeUnit / 1000000.0);
                    if (time == 0)
                    {
                        _shared->lastError = C::SETTIME_ERROR;
                        return FALSE;
                    }
                    if (time > 65535)
                    {
                        time = time / _timeBaseDB;
                        if (time > 65535)
                        {
                            time = time / _timeBaseDB;
                            if (time > 65535)
                            {
                                _shared->lastError = C::COMMTIME_OUT;
                                return FALSE;
                            }
                            delayWord = C::DELAYLONG;
                        }
                        else
                        {
                            delayWord = C::DELAYTIME;
                        }
                    }
                    timeBuff[0] = JM_HIGH_BYTE(time);
                    timeBuff[1] = JM_LOW_BYTE(time);
                    if (timeBuff[0] == 0)
                    {
                        if (_shared->isDoNow)
                        {
                            return commboxDo(delayWord, 1, timeBuff + 1);
                        }
                        return addToBuff(delayWord, 1, timeBuff + 1);
                    }
                    if (_shared->isDoNow)
                    {
                        return commboxDo(delayWord, 2, timeBuff);
                    }
                    return addToBuff(delayWord, 2, timeBuff);
                }

                gboolean sendOutData( const guint8 *buffer, size_t count)
                {
                    if (count == 0)
                    {
                        _shared->lastError = C::ILLIGICAL_LEN;
                        return FALSE;
                    }
                    if (_shared->isDoNow)
                    {
                        return commboxDo(C::SEND_DATA, count, buffer);
                    }
                    else
                    {
                        return addToBuff(C::SEND_DATA, count, buffer);
                    }
                }

                gboolean runBatch(guint8 *buffID, size_t count, gboolean isExecuteMany)
                {
                    guint8 commandWord = C::D0_BAT;
                    size_t i;

                    for (i = 0; i < count; i++)
                    {
                        if (_buffAdd[buffID[i]] == C::NULLADD)
                        {
                            _shared->lastError = C::NOUSED_BUFF;
                            return FALSE;
                        }
                    }
                    if (isExecuteMany)
                    {
                        commandWord = C::D0_BAT_FOR;
                    }
                    if (commandWord == C::D0_BAT && 
                        buffID[0] == _buffUsed[0])
                    {
                        commandWord = C::DO_BAT_00;
                        return commboxDo(commandWord, 0, NULL);
                    }

                    return commboxDo(commandWord, count, buffID);
                }

                size_t readData(guint8 *buff, size_t count, gint64 microseconds)
                {
                    jm_commbox_port_set_read_timeout(microseconds);

                    return jm_commbox_port_read(buff, count);
                }

                size_t readBytes(guint8 *buff, size_t count)
                {
                    return readData(buff, count, _shared->resWaitTime);
                }

                gboolean checkResult(gint64 microseconds)
                {
                    guint8 receiveBuffer;

                    jm_commbox_port_set_read_timeout(microseconds);

                    if (jm_commbox_port_read(&receiveBuffer, 1) != 1)
                    {
                        _shared->lastError = C::TIMEOUT_ERROR;
                    }

                    if (receiveBuffer == C::SUCCESS)
                    {
                        return TRUE;
                    }

                    while (jm_commbox_port_read(&receiveBuffer, 1) == 1);
                    _shared->lastError = receiveBuffer;
                    return FALSE;
                }

                gboolean stopNow(gboolean isStopExecute)
                {
                    if (isStopExecute)
                    {
                        guint8 receiveBuffer = 0;
                        guint32 times = C::REPLAYTIMES;
                        while (times--)
                        {
                            if (!commboxDo(C::STOP_EXECUTE, 0, NULL))
                            {
                                return FALSE;
                            }
                            if (jm_commbox_port_read(&receiveBuffer, 1) != 1)
                            {
                                _shared->lastError = C::TIMEOUT_ERROR;
                            }
                            if (receiveBuffer == C::RUN_ERR)
                            {
                                return TRUE;
                            }
                            _shared->lastError = C::TIMEOUT_ERROR;
                        }
                        return FALSE;
                    }
                    return commboxDo(C::STOP_REC, 0, NULL);
                }
            private:
                gboolean checkIdle()
                {
                    guint8 receiveBuffer = C::SUCCESS;
                    size_t avail;

                    avail = jm_commbox_port_bytes_available();

                    if (avail > 240)
                    {
                        jm_commbox_port_discard_in_buffer();
                        jm_commbox_port_discard_out_buffer();
                        return TRUE;
                    }

                    while (avail)
                    {
                        jm_commbox_port_read(&receiveBuffer, 1);
                        avail--;
                    }

                    if (receiveBuffer == C::SUCCESS)
                    {
                        return TRUE;
                    }

                    jm_commbox_port_set_read_timeout(JM_TIMER_TO_MS(200));

                    if (jm_commbox_port_read(&receiveBuffer, 1) != 1)
                    {
                        _shared->lastError = C::KEEPLINK_ERROR;
                        return FALSE;
                    }

                    if (receiveBuffer == C::SUCCESS)
                    {
                        return TRUE;
                    }

                    _shared->lastError = receiveBuffer;
                    return FALSE;
                }

                gboolean sendOK(gint64 microseconds)
                {
                    guint8 receiveBuffer = 0;

                    jm_commbox_port_set_read_timeout(microseconds);

                    while (TRUE)
                    {
                        if (jm_commbox_port_read(&receiveBuffer, 1) != 1)
                        {
                            _shared->lastError = C::TIMEOUT_ERROR;
                            return FALSE;
                        }

                        if (receiveBuffer == C::SEND_OK)
                        {
                            return TRUE;
                        }
                        else if (receiveBuffer >= C::UP_TIMEOUT && 
                            receiveBuffer <= C::ERROR_REC)
                        {
                            _shared->lastError = C::SENDDATA_ERROR;
                            return FALSE;
                        }
                    }

                    _shared->lastError = C::SENDDATA_ERROR;
                    return FALSE;
                }

                guint32 getBoxVer()
                {
                    return (_version[0] << 8) | (_version[1]);
                }

                size_t bufferSize()
                {
                    return _buffAdd[C::LINKBLOCK] - _buffAdd[C::SWAPBLOCK];
                }

                gboolean sendDataToEcuNew(size_t count, const guint8 *buff)
                {
                    size_t i;
                    size_t checksum = count + 5;
                    size_t size = count + 6;
                    guint8 *command = (guint8*)g_malloc(size);

                    command[0] = C::WR_DATA + _headPassword;
                    command[1] = JM_LOW_BYTE(count + 3);
                    command[2] = _buffAdd[C::SWAPBLOCK];
                    command[3] = C::SEND_CMD;
                    command[4] = JM_LOW_BYTE(count - 1);

                    command[checksum] = C::WR_DATA + command[1] + command[2] + command[3] + command[4];

                    memcpy(command + 5, buff, count);

                    for(i = 0; i < count; i++)
                    {
                        command[checksum] += buff[i];
                    }

                    for (i = 0; i < 3; i++)
                    {
                        if (!checkIdle() || 
                            (jm_commbox_port_write(command, size) != size))
                        {
                            _shared->lastError = C::SENDDATA_ERROR;
                            continue;
                        }

                        if (sendOK(20 * (count + 12)))
                        {
                            g_free(command);
                            return TRUE;
                        }
                    }

                    g_free(command);
                    return FALSE;
                }

                gboolean sendDataToEcuOld(size_t count, const guint8 *buff)
                {
                    size_t i;
                    size_t checksum = count + 4;
                    size_t size = count + 5;
                    guint8 *command = (guint8*)g_malloc(size);

                    command[0] = C::WR_DATA + _headPassword;
                    command[1] = JM_LOW_BYTE(count + 2);
                    command[2] = _buffAdd[C::SWAPBLOCK];
                    command[3] = JM_LOW_BYTE(count - 1);

                    command[checksum] = C::WR_DATA + command[1] + command[2] + command[3];

                    memcpy(command + 4, buff, count);

                    for (i = 0; i < count; i++)
                    {
                        command[checksum] += buff[i];
                    }

                    for (i = 0; i < 3; i++)
                    {
                        if (!checkIdle() || 
                            (jm_commbox_port_write(command, size) != size))
                        {
                            _shared->lastError = C::SENDDATA_ERROR;
                            continue;
                        }

                        if (sendOK(20 * (size + 6)))
                        {
                            g_free(command);
                            return TRUE;
                        }
                    }

                    g_free(command);
                    return FALSE;

                }

                gboolean sendDataToEcu(guint8 commandWord, size_t count, const guint8 *buff)
                {
                    if (commandWord == C::SEND_DATA && 
                        count <= C::SEND_LEN)
                    {
                        if (bufferSize() < (count + 1))
                        {
                            _shared->lastError = C::NOBUFF_TOSEND;
                            return FALSE;
                        }

                        if (getBoxVer() > 0x400)
                        {
                            // 增加发送长命令 
                            if (!sendDataToEcuNew(count, buff))
                            {
                                return FALSE;
                            }
                        }
                        else
                        {
                            // 保持与旧盒子兼容 
                            if (!sendDataToEcuOld(count, buff))
                            {
                                return FALSE;
                            }
                        }
                        return commboxDo(C::D0_BAT, 1, _buffAdd + C::SWAPBLOCK);
                    }
                    _shared->lastError = C::ILLIGICAL_LEN;
                    return FALSE;
                }

                gboolean commboxCommand(guint8 commandWord, size_t count, const guint8 *buff)
                {
                    size_t size = count + 2;
                    size_t csPos = count + 1;
                    size_t i;
                    guint8 *command = (guint8*)g_malloc(size);
                    guint8 *checksum = &command[csPos];

                    *checksum = JM_LOW_BYTE(commandWord + count);

                    if (commandWord < C::WR_DATA)
                    {
                        if (count == 0)
                        {
                            _shared->lastError = C::ILLIGICAL_LEN;
                            g_free(command);
                            return FALSE;
                        }
                        (*checksum)--;
                    }
                    else
                    {
                        if (count != 0)
                        {
                            _shared->lastError = C::ILLIGICAL_LEN;
                            g_free(command);
                            return FALSE;
                        }
                    }

                    command[0] = *checksum + _headPassword;
                    memcpy(command + 1, buff, count);

                    for (i = 0; i < count; i++)
                    {
                        *checksum += buff[i];
                    }

                    for (i = 0; i < 3; i++)
                    {
                        if (commandWord != C::STOP_REC && 
                            commandWord != C::STOP_EXECUTE)
                        {
                            if (!checkIdle() || 
                                (jm_commbox_port_write(command, size) != size))
                            {
                                _shared->lastError = C::SENDDATA_ERROR;
                                continue;
                            }
                        }
                        else
                        {
                            if (jm_commbox_port_write(command, size) != size)
                            {
                                _shared->lastError = C::SENDDATA_ERROR;
                                continue;
                            }
                        }

                        if (sendOK(JM_TIMER_TO_MS(200) * size))
                        {
                            g_free(command);
                            return TRUE;
                        }
                    }

                    g_free(command);
                    return FALSE;
                }

                gboolean commboxDo(guint8 commandWord, size_t count, const guint8 *buff)
                {
                    if (count > C::CMD_DATALEN)
                    {
                        return sendDataToEcu(commandWord, count, buff);
                    }
                    else
                    {
                        return commboxCommand(commandWord, count, buff);
                    }
                }

                gboolean doSet(guint8 commandWord, size_t count, const guint8 *buff)
                {
                    guint32 times = C::REPLAYTIMES;

                    while (times--)
                    {
                        if (!commboxDo(commandWord, count, buff))
                        {
                            continue;
                        }
                        if (checkResult(JM_TIMER_TO_MS(50)))
                        {
                            return TRUE;
                        }
                        stopNow(TRUE);
                    }
                    return FALSE;
                }

                size_t getCmdData(guint8 command, guint8 *receiveBuffer)
                {
                    guint8 checksum = command;
                    guint8 cmdInfo[2];
                    size_t i;

                    if (readData(cmdInfo, 2, JM_TIMER_TO_MS(150)) != 2)
                    {
                        return 0;
                    }

                    if (cmdInfo[0] != command)
                    {
                        _shared->lastError = cmdInfo[0];
                        jm_commbox_port_discard_in_buffer();
                        return 0;
                    }

                    if ((readData(receiveBuffer, cmdInfo[1], JM_TIMER_TO_MS(150)) != cmdInfo[1]) ||
                        (readData(cmdInfo, 1, JM_TIMER_TO_MS(150)) != 1))
                    {
                        return 0;
                    }

                    checksum += cmdInfo[1];

                    for (i = 0; i < cmdInfo[1]; i++)
                    {
                        checksum += receiveBuffer[i];
                    }

                    if (checksum != cmdInfo[0])
                    {
                        _shared->lastError = C::CHECKSUM_ERROR;
                        return 0;
                    }
                    return cmdInfo[1];
                }

                gboolean checkBox()
                {
                    static guint8 password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 0x17, 0x2D, 0x4D};
                    guint8 temp[5];
                    size_t i;
                    guint8 checksum;

                    srand((guint32)time(NULL));

                    temp[4] = 0x00;
                    i = 0;
                    while (i < 4)
                    {
                        temp[i] = JM_LOW_BYTE(rand());
                        temp[4] += temp[i];
                        i++;
                    }

                    if (jm_commbox_port_write(temp, 5) != 5)
                    {
                        _shared->lastError = C::SENDDATA_ERROR;
                        return FALSE;
                    }

                    i = 0;
                    checksum = temp[4] + temp[4];

                    while (i < sizeof(password))
                    {
                        checksum += password[i] ^ temp[i % 5];
                        i++;
                    }

                    g_usleep(JM_TIMER_TO_MS(20));

                    if (getCmdData(C::GETINFO, temp) <= 0)
                    {
                        return FALSE;
                    }

                    _headPassword = temp[0];

                    if (checksum != _headPassword)
                    {
                        _shared->lastError = C::CHECKSUM_ERROR;
                        return FALSE;
                    }
                    if (_headPassword == 0)
                    {
                        _headPassword = 0x55;
                    }
                    return TRUE;
                }

                gboolean initBox()
                {
                    guint32 length;
                    size_t pos = 0;
                    size_t i;

					_headPassword = 0x00;
                    _shared->isDB20 = FALSE;

                    if (!commboxDo(C::GETINFO, 0, NULL))
                    {
                        return FALSE;
                    }

                    length = getCmdData(C::GETINFO, _cmdTemp);
                    if (length < C::COMMBOXIDLEN)
                    {
                        _shared->lastError = C::LOST_VERSIONDATA;
                        return FALSE;
                    }

                    _timeUnit = 0;
                    for (i = 0; i < C::MINITIMELEN; i++)
                    {
                        _timeUnit = (_timeUnit << 8) | _cmdTemp[pos++];
                    }
                    _timeBaseDB = _cmdTemp[pos++];
                    _timeExternDB = _cmdTemp[pos++];
                    _cmdBuffLen = _cmdTemp[pos++];
                    if (_timeBaseDB == 0 ||
                        _timeUnit == 0 ||
                        _cmdBuffLen == 0)
                    {
                        _shared->lastError = C::COMMTIME_ZERO;
                        return FALSE;
                    }

                    for (i = 0; i < C::COMMBOXIDLEN; i++)
                    {
                        _boxID[i] = _cmdTemp[pos++];
                    }

                    for (i = 0; i < C::VERSIONLEN; i++)
                    {
                        _version[i] = _cmdTemp[pos++];
                    }

                    _port[0] = C::NULLADD;
                    _port[1] = C::NULLADD;
                    _port[2] = C::NULLADD;
                    _port[3] = C::NULLADD;

                    _buffID = C::NULLADD;
                    _buffUsedNum = 0;

                    memset(_buffAdd, C::NULLADD, C::MAXIM_BLOCK);
                    _buffAdd[C::LINKBLOCK] = _cmdBuffLen;
                    _buffAdd[C::SWAPBLOCK] = 0;
                    return TRUE;
                }

                gboolean setRF(guint8 cmd, guint8 cmdInfo)
                {
                    size_t times = C::REPLAYTIMES;

                    cmdInfo += cmd;
                    if (cmd == C::SETRFBAUD)
                    {
                        times = 2;
                    }

                    g_usleep(JM_TIMER_TO_MS(6));
                    while (times--)
                    {
                        if (checkIdle() && 
                            (jm_commbox_port_write(&cmdInfo, 1) == 1))
                        {
                            if (!sendOK(JM_TIMER_TO_MS(500)))
                            {
                                continue;
                            }

                            if ((jm_commbox_port_write(&cmdInfo, 1) != 1) ||
                                !checkResult(JM_TIMER_TO_MS(500)))
                            {
                                continue;
                            }
                            g_usleep(JM_TIMER_TO_MS(500));
                            return TRUE;
                        }
                    }
                    return FALSE;
                }

                gboolean doSetPCBaud(guint8 baud)
                {
                    JM::SerialPort *port = (JM::SerialPort*)jm_commbox_port_get_pointer();

                    if (!commboxDo(C::SET_UPBAUD, 1, &baud))
                    {
                        return FALSE;
                    }

                    g_usleep(JM_TIMER_TO_MS(500));
                    checkResult(JM_TIMER_TO_MS(500));
                    setRF(C::SETRFBAUD, baud);
                    checkResult(JM_TIMER_TO_MS(500));

                    switch (baud)
                    {
                    case C::UP_9600BPS:
                        _shared->lastError = port->setBaudrate(9600);
                        break;
                    case C::UP_19200BPS:
                        _shared->lastError = port->setBaudrate(19200);
                        break;
                    case C::UP_38400BPS:
                        _shared->lastError = port->setBaudrate(38400);
                        break;
                    case C::UP_57600BPS:
                        _shared->lastError = port->setBaudrate(57600);
                        break;
                    case C::UP_115200BPS:
                        _shared->lastError = port->setBaudrate(115200);
                        break;
                    default:
                        _shared->lastError = C::ILLIGICAL_CMD;
                        return FALSE;
                        break;
                    }
                    if (_shared->lastError)
                    {
                        _shared->lastError = C::DISCONNECT_COMM;
                        return FALSE;
                    }
                    setRF(C::SETRFBAUD, baud);
                    if (!commboxDo(C::SET_UPBAUD, 1,&baud))
                        return FALSE;
                    if (!checkResult(JM_TIMER_TO_MS(500)))
                        return FALSE;
                    jm_commbox_port_discard_in_buffer();
                    return TRUE;
                }

                gboolean setPCBaud(guint8 baud)
                {
                    size_t times = C::REPLAYTIMES;
                    while (times--)
                    {
                        if (times == 0)
                            return FALSE;
                        if (doSetPCBaud(baud))
                            break;
                    }
                    return TRUE;
                }

                gboolean openBox()
                {
                    size_t i;

                    for (i = 0; i < 3; i++)
                    {
                        setRF(C::RESET_RF, 0);
                        setRF(C::SETDTR_L, 0);
                        if (initBox() && checkBox())
                        {
                            jm_commbox_port_discard_in_buffer();
                            if (jm_commbox_port_get_type() == JM_COMMBOX_PORT_SERIAL_PORT)
                            {
                                return setPCBaud(C::UP_57600BPS);
                            }
                            return TRUE;
                        }
                    }

                    return FALSE;
                }

            private:
                guint32 _buffUsedNum;
                guint32 _timeUnit;
                guint32 _timeBaseDB;
                guint32 _timeExternDB;
                guint32 _cmdBuffLen;
                guint8 _buffID;
                guint8 _headPassword;
                guint8 _buffAdd[C::MAXIM_BLOCK + 2];
                guint8 _buffUsed[C::MAXIM_BLOCK];
                guint8 _version[C::VERSIONLEN];
                guint8 _boxID[C::COMMBOXIDLEN];
                guint8 _port[C::COMMBOXPORTNUM];
                guint8 _cmdTemp[256]; // 写入命令缓冲区 
            };
        }
    }
}

#endif