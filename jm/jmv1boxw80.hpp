#ifndef __JM_V1_BOX_W80_HPP__
#define __JM_V1_BOX_W80_HPP__

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
        namespace W80
        {
            class Box : public V1::Box<W80::CONSTANT>
            {
            public:
                Box(V1::Shared *shared)
                    : V1::Box<W80::CONSTANT>(shared)
                {
                }

                size_t readData(guint8 *buff, size_t count, gint64 microseconds)
                {
                    size_t avail = 0;
                    size_t len = 0;
                    jm_commbox_port_set_read_timeout(microseconds);
                    len = jm_commbox_port_read(buff, count);
                    if (len < count)
                    {
                        avail = jm_commbox_port_bytes_available();
                        if (avail > 0)
                        {
                            if (avail <= (count - len))
                            {
                                len += jm_commbox_port_read(buff + len, avail);
                            }
                            else
                            {
                                len += jm_commbox_port_read(buff + len, count - len);
                            }
                        }
                    }
                    return len;
                }

                size_t readBytes(guint8 *buff, size_t count)
                {
                    return readData(buff, count, JM_TIMER_TO_MS(500));
                }

                gboolean setLineLevel(guint8 valueLow, guint8 valueHigh)
                {
                    _ports[1] &= ~valueLow;
                    _ports[1] |= valueHigh;

                    return doSet(C::SET_PORT1, 1, _ports + 1);
                }

                gboolean setCommCtrl(guint8 valueOpen, guint8 valueClose)
                {
                    _ports[2] &= ~valueOpen;
                    _ports[2] |= valueClose;
                    return doSet(C::SET_PORT2, 1, _ports + 2);
                }

                gboolean setCommLine(guint8 sendLine, guint8 recvLine)
                {
                    if (sendLine > 7)
                    {
                        sendLine = 0x0F;
                    }

                    if (recvLine > 7)
                    {
                        recvLine = 0x0F;
                    }

                    _ports[0] = sendLine | (recvLine << 4);
                    return doSet(C::SET_PORT0, 1, _ports);
                }

                gboolean turnOverOneByOne()
                {
                    return doSet(C::ONEBYONE, 0, NULL);
                }

                gboolean keepLink(gboolean isRunLink)
                {
                    if (isRunLink)
                    {
                        return doSet(C::RUN_LINK, 0, NULL);
                    }
                    else
                    {
                        return doSet(C::STOP_LINK, 0, NULL);
                    }
                }

                gboolean setCommLink(guint8 ctrlWord1, guint8 ctrlWord2, 
                    guint8 ctrlWord3)
                {
                    guint8 ctrlWord[3];
                    guint8 modeControl = ctrlWord1 & 0xE0;
                    size_t length = 3;

                    ctrlWord[0] = ctrlWord1;
                    if ((ctrlWord1 & 0x04) != 0)
                    {
                        _shared->is_db_20 = TRUE;
                    }
                    else
                    {
                        _shared->is_db_20 = FALSE;
                    }

                    if (modeControl == C::SET_VPW || 
                        modeControl == C::SET_PWM)
                    {
                        return doSet(C::SET_CTRL, 1, ctrlWord);
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
                        return FALSE;
                    }

                    return doSet(C::SET_CTRL, length, ctrlWord);
                }

                gboolean setCommBaud(gdouble baud)
                {
                    guint8 baudTime[2];
                    gdouble instructNum = 0;

                    instructNum = ((1000000.0 / _boxTimeUnit) * 1000000) / 
                        baud;

                    if (_shared->is_db_20)
                    {
                        instructNum /= 20;
                    }

                    instructNum += 0.5;
                    if (instructNum > 65535 || instructNum < 10)
                    {
                        return FALSE;
                    }

                    baudTime[0] = JM_HIGH_BYTE(instructNum);
                    baudTime[1] = JM_LOW_BYTE(instructNum);

                    if (baudTime[0] == 0)
                    {
                        return doSet(C::SET_BAUD, 1, baudTime + 1);
                    }
                    return doSet(C::SET_BAUD, 2, baudTime);
                }

                gboolean setCommTime(guint8 type, guint32 time)
                {
                    guint8 timeBuff[2];

                    getLinkTime(type, time);

                    if (type == C::SETVPWSTART || 
                        type == C::SETVPWRECS)
                    {
                        if (type == C::SETVPWRECS)
                        {
                            time = (time * 2) / 3;
                        }
                        type = type + (C::SETBYTETIME & 0xF0);
                        time = (guint32)(time / (_boxTimeUnit / 1000000.0));
                    }
                    else
                    {
                        time = (guint32)((time / _timeBaseDB) / 
                            (_boxTimeUnit / 1000000.0));
                    }

                    timeBuff[0] = JM_HIGH_BYTE(time);
                    timeBuff[1] = JM_LOW_BYTE(time);
                    if (timeBuff[0] == 0)
                    {
                        return doSet(type, 1, timeBuff + 1);
                    }
                    return doSet(type, 2, timeBuff);
                }

                gboolean commboxDelay(guint32 time)
                {
                    guint8 timeBuff[2];
                    guint8 delayWord = C::DELAYSHORT;

                    time = (guint32)(time / (_boxTimeUnit / 1000000.0));

                    if (time == 0)
                    {
                        return FALSE;
                    }

                    if (time > 65535)
                    {
                        time = (guint32)(time / _timeBaseDB);

                        if (time > 65535)
                        {
                            time = (guint32)((time * _timeBaseDB) / 
                                _timeExternDB);
                            if (time > 65535)
                            {
                                return FALSE;
                            }
                            delayWord = C::DELAYDWORD;
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
                        return doSet(delayWord, 1, timeBuff + 1);
                    }
                    return doSet(delayWord, 2, timeBuff);
                }

                gboolean sendOutData(const guint8 *buffer, size_t count)
                {
                    return doSet(C::SEND_DATA, count, buffer);
                }

                gboolean runReceive(guint8 type)
                {
                    if (type == C::GET_PORT1)
                    {
                        _shared->is_db_20 = FALSE;
                    }
                    return doCmd(type, 0, NULL);
                }

                gboolean stopNow(gboolean isStopExecute)
                {
                    guint8 cmd;
                    size_t i;

                    if (isStopExecute)
                    {
                        cmd = C::STOP_EXECUTE;
                    }
                    else
                    {
                        cmd = C::STOP_REC;
                    }

                    for (i = 0; i < 3; i++)
                    {
                        if (jm_commbox_port_write(&cmd, 1) != 1)
                            continue;

                        if (checkSend())
                        {
                            if (isStopExecute && 
                                !checkResult(JM_TIMER_TO_MS(200)))
                                continue;
                            return TRUE;
                        }
                    }
                    return FALSE;
                }

                gboolean openComm()
                {
                    if (jm_commbox_port_get_type() == 
                        JM_COMMBOX_PORT_SERIAL_PORT)
                    {
                        JM::SerialPort *port = 
                            (JM::SerialPort*)jm_commbox_port_get_pointer();
                        JMStringArray *vec = NULL;
                        size_t vec_length = 0;
                        size_t i;

                        if (port == NULL)
                            return FALSE;

                        vec = JM::SerialPort::getSystemPorts();
                        vec_length = jm_string_array_size(vec);

                        for (i = 0; i < vec_length; i++)
                        {
                            port->setPortName(jm_string_array_get(vec, i));
                            port->setBaudrate(115200);
                            port->setStopbits(JM_SP_SB_TWO);
                            port->setParity(JM_SP_PAR_NONE);
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
                    reset();
                    setRF(C::RF_RESET, 0);
                    return TRUE;
                }

                guint8 getAbsAdd(guint8 buffID, guint8 add)
                {
                    return 0;
                }

                gboolean updateBuff(guint8 type, guint8 *buffer)
                {
                    int len = 0;
                    guint8 buf[3];
                    buf[0] = buffer[0];
                    buf[1] = buffer[1];

                    if ((type == C::INC_BYTE) || 
                        (type == C::DEC_BYTE) || 
                        (type == C::INVERTBYTE))
                    {
                        len = 1;
                    }
                    else if ((type == C::UPDATE_BYTE) || 
                        (type == C::ADD_BYTE) || 
                        (type == C::SUB_BYTE))
                    {
                        len = 2;
                    }
                    else
                    {
                        len = 3;
                    }
                    return doSet(type, len, buf);
                }

                gboolean newBatch(guint8 buffID)
                {
                    _pos = 0;
                    _isLink = (buffID == C::LINKBLOCK ? TRUE : FALSE);
                    _shared->is_do_now = FALSE;
                    return TRUE;
                }

                gboolean endBatch()
                {
                    int i = 0;

                    _shared->is_do_now = TRUE;
                    _buf[_pos++] = 0; // 命令块以0x00标记结束 
                    if (_isLink)
                    {
                        // 修改update_buff使用到的地址 
                        while (_buf[i] != 0)
                        {
                            guint8 mode = _buf[i] & 0xFC;
                            if (mode == C::COPY_BYTE)
                            {
                                _buf[i + 3] += C::MAXBUFF_LEN - _pos;
                            }
                            else if (mode == C::SUB_BYTE)
                            {
                                _buf[i + 2] += C::MAXBUFF_LEN - _pos;
                            }
                            else
                            {
                                _buf[i + 1] += C::MAXBUFF_LEN - _pos;
                            }

                            if (_buf[i] == C::SEND_DATA)
                            {
                                i += 1 + _buf[i + 1] + 1 + 1;
                            }
                            else if(_buf[i] >= C::REC_LEN_1 && 
                                _buf[i] <= C::REC_LEN_15)
                            {
                                i++; // 特殊 
                            }
                            else
                            {
                                i = 1 + ((_buf[i] & 0x03) + 1);
                            }
                        }
                    }

                    return doCmd(C::WR_DATA, _pos, _buf);
                }

                gboolean delBatch(guint8 buffID)
                {
                    _shared->is_do_now = TRUE;

                    _pos = 0;
                    return TRUE;

                }

                gboolean runBatch(guint8 *buffID, size_t count, 
                    gboolean isExecuteMany)
                {
                    guint8 cmd;

                    if (buffID[0] == C::LINKBLOCK)
                    {
                        if (isExecuteMany)
                        {
                            cmd = C::DO_BAT_LN;
                        }
                        else
                        {
                            cmd = C::DO_BAT_L;
                        }
                    }
                    else
                    {
                        if (isExecuteMany)
                        {
                            cmd = C::DO_BAT_CN;
                        }
                        else
                        {
                            cmd = C::DO_BAT_C;
                        }
                    }
                    return doCmd(cmd, 0, NULL);
                }

            private:
                gboolean checkIdle()
                {
                    guint8 rb = C::READY;
                    gint32 avail;

                    avail = jm_commbox_port_bytes_available();

                    if (avail > 20)
                    {
                        jm_commbox_port_discard_in_buffer();
                        return TRUE;
                    }
                    while (jm_commbox_port_bytes_available())
                    {
                        jm_commbox_port_read(&rb, 1);
                    }

                    if (rb == C::READY || rb == C::ERR)
                    {
                        return TRUE;
                    }

                    if (jm_commbox_port_read(&rb, 1) != 1)
                    {
                        return FALSE;
                    }

                    if (rb == C::READY || rb == C::ERR)
                    {
                        return TRUE;
                    }

                    return FALSE;
                }

                gboolean checkSend()
                {
                    guint8 rb = 0;
                    if (jm_commbox_port_read(&rb, 1) != 1)
                    {
                        return FALSE;
                    }

                    if (rb == C::RECV_OK)
                    {
                        return TRUE;
                    }

                    return FALSE;
                }

                gboolean checkResult(gint64 microseconds)
                {
                    guint8 rb = 0;

                    if (jm_commbox_port_read(&rb, 1) != 1)
                    {
                        return FALSE;
                    }

                    if (rb == C::READY || rb == C::ERR)
                    {
                        jm_commbox_port_discard_in_buffer();
                        return TRUE;
                    }

                    return FALSE;
                }

                gboolean sendCmd(guint8 cmd, size_t count, const guint8 *data)
                {
                    GByteArray *command = NULL;
                    size_t i;
                    guint8 cs = cmd;

                    cmd += _runFlag;

                    command = g_byte_array_new();
                    g_byte_array_append(command, &cmd, 1);

                    g_byte_array_append(command, data, count);

                    for (i = 0; i < count; i++)
                    {
                        cs += data[i];
                    }

                    g_byte_array_append(command, &cs, 1);

                    for (i = 0; i < 3; i++)
                    {
                        if (!checkIdle())
                        {
                            continue;
                        }

                        if (jm_commbox_port_write(command->data, command->len) != 
                            command->len)
                        {
                            continue;
                        }

                        if (checkSend())
                        {
                            return TRUE;
                        }
                    }
                    return FALSE;
                }

                gboolean doCmd(guint8 cmd, size_t count, const guint8 *buff)
                {
                    gboolean ret = FALSE;

                    _startPos = 0;

                    if (cmd != C::WR_DATA && cmd != C::SEND_DATA)
                    {
                        cmd |= count; // 加上长度位 
                    }

                    if (_shared->is_do_now)
                    {
                        // 发送到BOX执行 
                        if (cmd == C::WR_DATA)
                        {
                            GByteArray *temp = NULL;
                            if (count == 0)
                            {
                                return FALSE;
                            }

                            temp = g_byte_array_new();
                            if (_isLink)
                            {
                                cmd = 0xFF; // 写链路保持 
                            }
                            else
                            {
                                cmd = 0x00; // 写通讯命令 
                            }
                            g_byte_array_append(temp, &cmd, 1);
                            cmd = count;
                            g_byte_array_append(temp, &cmd, 1);
                            g_byte_array_append(temp, buff, count);
                            ret = sendCmd(C::WR_DATA, 
                                temp->len, temp->data);
                            g_byte_array_free(temp, TRUE);
                            return ret;
                        }
                        else if (cmd == C::SEND_DATA)
                        {
                            GByteArray *temp = NULL;
                            if (count == 0)
                                return FALSE;
                            temp = g_byte_array_new();
                            cmd = 0x00; // 写入位置 
                            g_byte_array_append(temp, &cmd, 1);
                            cmd = count + 2;
                            g_byte_array_append(temp, &cmd, 1); // 数据包长度 
                            cmd = C::SEND_DATA; // 命令 
                            g_byte_array_append(temp, &cmd, 1);
                            cmd = count - 1; // 命令长度-1 
                            g_byte_array_append(temp, &cmd, 1);
                            g_byte_array_append(temp, buff, count);
                            ret = sendCmd(C::WR_DATA, 
                                temp->len, temp->data);
                            g_byte_array_free(temp, TRUE);
                            if (!ret)
                            {
                                return FALSE;
                            }
                            return sendCmd(C::DO_BAT_C, 0,
                                NULL);
                        }
                        else
                        {
                            return sendCmd(cmd, count, buff);
                        }
                    }
                    else
                    {
                        // 写命令到缓冲区 
                        _buf[_pos++] = cmd;
                        if (cmd == C::SEND_DATA)
                        {
                            _buf[_pos++] = count - 1;
                        }
                        _startPos = _pos;
                        if (count > 0)
                        {
                            memcpy(_buf + _pos, buff, count);
                        }
                        _pos += count;
                        return TRUE;
                    }
                    return FALSE;
                }

                gboolean doSet(guint8 cmd, size_t count, const guint8 *buff)
                {
                    gboolean result = FALSE;

                    result = doCmd(cmd, count, buff);
                    if (result && _shared->is_do_now)
                    {
                        result = checkResult(JM_TIMER_TO_MS(150));
                    }
                    return result;
                }

                guint8 getBuffData(guint8 addr, guint8 *buff, size_t count)
                {
                    guint8 temp[2];
                    temp[0] = addr;
                    temp[1] = count;

                    if (!doCmd(C::GET_BUF, 2, temp))
                    {
                        return 0;
                    }

                    return getCmdData(buff, count);
                }

                gboolean initBox()
                {
                    static guint8 password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 0x17, 0x2D, 0x4D};
                    guint8 buf[32];
                    size_t i;
                    guint8 run = 0;

                    srand((guint8)time(NULL));
                    _shared->is_do_now = TRUE;
                    _runFlag = 0;

                    for (i = 0; i < 4; i++)
                    {
                        buf[i] = JM_LOW_BYTE(rand());
                    }

                    for (i = 0; i < 10; i++)
                    {
                        run += password[i] ^ buf[i % 3 + 1];
                    }

                    if (run == 0)
                    {
                        run = 0x55;
                    }

                    if (!doCmd(C::GET_CPU, 3, buf + 1))
                    {
                        return FALSE;
                    }
                    if (getCmdData(buf, 32) <= 0)
                    {
                        return FALSE;
                    }
                    _runFlag = 0; // Run 
                    _boxTimeUnit = 0;
                    for (i = 0; i < 3; i++)
                    {
                        _boxTimeUnit = (_boxTimeUnit << 8) | buf[i];
                    }
                    _timeBaseDB = buf[i++];
                    _timeExternDB = buf[i++];

                    for (i = 0; i < C::MAXPORT_NUM; i++)
                    {
                        _ports[i] = 0xFF;
                    }

                    _pos = 0;
                    _shared->is_db_20 = FALSE;
                    return TRUE;
                }

                guint8 getCmdData(guint8 *receiveBuffer, size_t count)
                {
                    guint8 len = 0;
                    guint8 cs = 0;

                    if ((readBytes(receiveBuffer, 1) != 1)
                        || (readBytes(&len, 1) != 1))
                    {
                        return 0;
                    }

                    if (len > count)
                    {
                        len = count;
                    }

                    if (readBytes(receiveBuffer, len) != len)
                    {
                        return 0;
                    }

                    if (readBytes(&cs, 1) != 1)
                    {
                        return 0;
                    }

                    return len;
                }

                gboolean checkBox()
                {
                    guint8 buf[32];

                    if (!doCmd(C::GET_BOXID, 0, NULL))
                    {
                        return FALSE;
                    }

                    if (getCmdData(buf, 32) <= 0)
                    {
                        return FALSE;
                    }

                    _boxVer = (buf[10] << 8) | buf[11];
                    return TRUE;
                }

                void getLinkTime(guint8 type, guint32 time)
                {
                    if (type == C::SETBYTETIME)
                    {
                        _shared->req_byte_to_byte = time;
                    }
                    else if (type == C::SETRECBBOUT)
                    {
                        _shared->req_wait_time = time;
                    }
                    else if (type == C::SETRECFROUT)
                    {
                        _shared->res_byte_to_byte = time;
                    }
                    else
                    {
                        _shared->res_wait_time = time;
                    }
                }

                gboolean setRF(guint8 cmd, 
                    guint8 data)
                {
                    return FALSE;
                }

                gboolean openBox()
                {
                    if (initBox() && checkBox())
                    {
                        jm_commbox_port_discard_in_buffer();
                        jm_commbox_port_discard_out_buffer();
                        return TRUE;
                    }
                    return FALSE;
                }

                gboolean reset()
                {
                    size_t i;

                    stopNow(TRUE);
                    jm_commbox_port_discard_out_buffer();
                    jm_commbox_port_discard_in_buffer();

                    for (i = 0; i < C::MAXPORT_NUM; i++)
                    {
                        _ports[i] = 0xFF;
                    }

                    return doCmd(C::RESET, 0, NULL);
                }

                gboolean copyBuff(guint8 dest, guint8 src, guint8 len)
                {
                    guint8 buf[3];

                    buf[0] = dest;
                    buf[1] = src;
                    buf[2] = len;

                    return doSet(C::COPY_BYTE, 3, buf);
                }

                guint32 getBoxVer()
                {
                    return _boxVer;
                }

            private:
                guint32 _boxTimeUnit;
                guint32 _timeBaseDB;
                guint32 _timeExternDB;
                guint32 _pos;
                gboolean _isLink;
                guint32 _runFlag;
                guint32 _boxVer;
                guint32 _startPos;
                guint8 _ports[C::MAXPORT_NUM];
                guint8 _buf[C::MAXBUFF_LEN];
            };
        }
    }
}

#endif