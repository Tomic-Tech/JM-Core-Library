﻿#ifndef __JM_DIAG_V1_W80_BOX_HPP__
#define __JM_DIAG_V1_W80_BOX_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/utils.hpp>
#include <jm/diag/v1/box.hpp>
#include <jm/diag/v1/w80/constant.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
namespace W80
{
class Box : public V1::Box<W80::Constant>
{
public:
    Box(const boost::shared_ptr<V1::Shared> &shared)
        : V1::Box<W80::Constant>(shared)
        , _pos(0)
    {
    }

    std::size_t readData(boost::uint8_t *buff, std::size_t count, boost::int64_t microseconds)
    {
        std::size_t avail = 0;
        std::size_t len = 0;
        boost::system::error_code ec;
        stream()->setReadTimeout(MicroSeconds(microseconds));
        len = stream()->read(MutableBuffer(buff, count), ec);
        if (len < count)
        {
            avail = stream()->bytesAvailable();
            if (avail > 0)
            {
                if (avail <= (count - len))
                {
                    len += stream()->read(MutableBuffer(buff + len, avail), ec);
                }
                else
                {
                    len += stream()->read(MutableBuffer(buff + len, count - len), ec);
                }
            }
        }
        return len;
    }

    std::size_t readBytes(boost::uint8_t *buff, std::size_t count)
    {
        return readData(buff, count, toMicroSeconds(MilliSeconds(500)));
    }

    bool setLineLevel(boost::uint8_t valueLow, boost::uint8_t valueHigh)
    {
        _ports[1] &= ~valueLow;
        _ports[1] |= valueHigh;

        return doSet(Constant::SET_PORT1, _ports.data() + 1, 1);
    }

    bool setCommCtrl(boost::uint8_t valueOpen, boost::uint8_t valueClose)
    {
        _ports[2] &= ~valueOpen;
        _ports[2] |= valueClose;
        return doSet(Constant::SET_PORT2, _ports.data() + 2, 1);
    }

    bool setCommLine(boost::uint8_t sendLine, boost::uint8_t recvLine)
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
        return doSet(Constant::SET_PORT0, _ports.data(), 1);
    }

    bool turnOverOneByOne()
    {
        return doSet(Constant::ONEBYONE, NULL, 0);
    }

    bool keepLink(bool isRunLink)
    {
        if (isRunLink)
        {
            return doSet(Constant::RUN_LINK, NULL, 0);
        }
        else
        {
            return doSet(Constant::STOP_LINK, NULL, 0);
        }
    }

    bool setCommLink(boost::uint8_t ctrlWord1, boost::uint8_t ctrlWord2, boost::uint8_t ctrlWord3)
    {
        boost::array<boost::uint8_t, 3> ctrlWord;
        boost::uint8_t modeControl = ctrlWord1 & 0xE0;
        std::size_t length = 3;

        ctrlWord[0] = ctrlWord1;
        if ((ctrlWord1 & 0x04) != 0)
        {
            _shared->isDB20 = true;
        }
        else
        {
            _shared->isDB20 = false;
        }

        if (modeControl == Constant::SET_VPW ||
                modeControl == Constant::SET_PWM)
        {
            return doSet(Constant::SET_CTRL, ctrlWord.data(), 1);
        }
        ctrlWord[1] = ctrlWord2;
        ctrlWord[2] = ctrlWord3;
        if (ctrlWord3 == 0)
        {
            --length;
            if (ctrlWord2 == 0)
            {
                --length;
            }
        }

        if (modeControl == Constant::EXRS_232 && length < 2)
        {
            return false;
        }

        return doSet(Constant::SET_CTRL, ctrlWord.data(), length);
    }

    bool setCommBaud(boost::uint32_t baud)
    {
        boost::array<boost::uint8_t, 2> baudTime;
        double instructNum = 0;

        instructNum = ((1000000.0 / _boxTimeUnit) * 1000000) / baud;

        if (_shared->isDB20)
        {
            instructNum /= 20;
        }

        instructNum += 0.5;
        if (instructNum > 65535 || instructNum < 10)
        {
            return false;
        }

        baudTime[0] = HighByte(instructNum);
        baudTime[1] = LowByte(instructNum);

        if (baudTime[0] == 0)
        {
            return doSet(Constant::SET_BAUD, baudTime.data() + 1, 1);
        }
        return doSet(Constant::SET_BAUD, baudTime.data(), 2);
    }

    bool setCommTime(boost::uint8_t type, boost::int64_t time)
    {
        boost::array<boost::uint8_t, 2> timeBuff;

        getLinkTime(type, time);

        if (type == Constant::SETVPWSTART ||
                type == Constant::SETVPWRECS)
        {
            if (type == Constant::SETVPWRECS)
            {
                time = (time * 2) / 3;
            }
            type = type + (Constant::SETBYTETIME & 0xF0);
            time = (boost::uint32_t)(time / (_boxTimeUnit / 1000000.0));
        }
        else
        {
            time = (boost::uint32_t)((time / _timeBaseDB) / (_boxTimeUnit / 1000000.0));
        }

        timeBuff[0] = HighByte(time);
        timeBuff[1] = LowByte(time);
        if (timeBuff[0] == 0)
        {
            return doSet(type, timeBuff.data() + 1, 1);
        }
        return doSet(type, timeBuff.data(), 2);
    }

    bool commboxDelay(boost::int64_t time)
    {
        boost::array<boost::uint8_t, 2> timeBuff;
        boost::uint8_t delayWord = Constant::DELAYSHORT;

        time = (boost::uint32_t)(time / (_boxTimeUnit / 1000000.0));

        if (time == 0)
        {
            return false;
        }

        if (time > 65535)
        {
            time = (boost::uint32_t)(time / _timeBaseDB);

            if (time > 65535)
            {
                time = (boost::uint32_t)((time * _timeBaseDB) / _timeExternDB);
                if (time > 65535)
                {
                    return false;
                }
                delayWord = Constant::DELAYDWORD;
            }
            else
            {
                delayWord = Constant::DELAYTIME;
            }
        }
        timeBuff[0] = HighByte(time);
        timeBuff[1] = LowByte(time);

        if (timeBuff[0] == 0)
        {
            return doSet(delayWord, timeBuff.data() + 1, 1);
        }
        return doSet(delayWord, timeBuff.data(), 2);
    }

    inline bool sendOutData(const boost::uint8_t *buffer, std::size_t count)
    {
        return doSet(Constant::SEND_DATA, buffer, count);
    }

    bool runReceive(boost::uint8_t type)
    {
        if (type == Constant::GET_PORT1)
        {
            _shared->isDB20 = false;
        }
        return doCmd(type, NULL, 0);
    }

    bool stopNow(bool isStopExecute)
    {
        boost::uint8_t cmd;
        std::size_t i;

        if (isStopExecute)
        {
            cmd = Constant::STOP_EXECUTE;
        }
        else
        {
            cmd = Constant::STOP_REC;
        }

        boost::system::error_code ec;
        for (i = 0; i < 3; ++i)
        {
            if (stream()->write(ConstBuffer(&cmd, 1), ec) != 1)
                continue;

            if (checkSend())
            {
                if (isStopExecute &&
                        !checkResult(toMicroSeconds(MilliSeconds(200))))
                    continue;
                return true;
            }
        }
        return false;
    }

    bool openComm()
    {
        if (commbox().type() == Commbox::SerialPort)
        {
            PhysicalStream<boost::asio::serial_port> *ss = commbox().physicalStream<boost::asio::serial_port>();

            if (ss == NULL)
                return false;

            boost::asio::serial_port &port = ss->physical();

            std::vector<std::string> vec = Asio::SerialPort::getSystemPorts();

            boost::system::error_code ec;
            for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
            {
                port.set_option(boost::asio::serial_port::baud_rate(115200), ec);
                port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::two), ec);
                port.set_option(boost::asio::serial_port::parity(), ec);
                port.set_option(boost::asio::serial_port::flow_control(), ec);
                port.set_option(boost::asio::serial_port::character_size(), ec);
                port.open(*it, ec);
                if (!ec)
                {
                    Asio::SerialPort::setDtr(port.native_handle(), true, ec);
                    if (openBox())
                        return true;
                }
                port.close(ec);
            }
        }

        return false;
    }

    bool closeComm()
    {
        boost::system::error_code ec;
        if (commbox().type() == Commbox::SerialPort)
        {
            PhysicalStream<boost::asio::serial_port> *ss = commbox().physicalStream<boost::asio::serial_port>();
            boost::asio::serial_port &port = ss->physical();
            if (!port.is_open())
                return true;
        }

        reset();
        setRF(Constant::RF_RESET, 0);

        if  (commbox().type() == Commbox::SerialPort)
        {
            PhysicalStream<boost::asio::serial_port> *ss = commbox().physicalStream<boost::asio::serial_port>();
            boost::asio::serial_port &port = ss->physical();
            port.close(ec);
        }
        return true;
    }

    boost::uint8_t getAbsAdd(boost::uint8_t buffID, boost::uint8_t add)
    {
        return 0;
    }

    bool updateBuff(boost::uint8_t type, boost::uint8_t *buffer)
    {
        int len = 0;
        boost::array<boost::uint8_t, 3> buf;
        buf[0] = buffer[0];
        buf[1] = buffer[1];

        if ((type == Constant::INC_BYTE) ||
                (type == Constant::DEC_BYTE) ||
                (type == Constant::INVERTBYTE))
        {
            len = 1;
        }
        else if ((type == Constant::UPDATE_BYTE) ||
                 (type == Constant::ADD_BYTE) ||
                 (type == Constant::SUB_BYTE))
        {
            len = 2;
        }
        else
        {
            len = 3;
        }
        return doSet(type, buf.data(), len);
    }

    bool newBatch(boost::uint8_t buffID)
    {
        _pos = 0;
        _isLink = (buffID == Constant::LINKBLOCK ? true : false);
        _shared->isDoNow = false;
        return true;
    }

    bool endBatch()
    {
        int i = 0;

        _shared->isDoNow = true;
        _buf[_pos++] = 0; //命令块以0x00标记结束
        if (_isLink)
        {
            //修改UpdateBuff使用到的地址
            while (_buf[i] != 0)
            {
                boost::uint8_t mode = _buf[i] & 0xFC;
                if (mode == Constant::COPY_BYTE)
                {
                    _buf[i + 3] += Constant::MAXBUFF_LEN - _pos;
                }
                else if (mode == Constant::SUB_BYTE)
                {
                    _buf[i + 2] += Constant::MAXBUFF_LEN - _pos;
                }
                else
                {
                    _buf[i + 1] += Constant::MAXBUFF_LEN - _pos;
                }

                if (_buf[i] == Constant::SEND_DATA)
                {
                    i += 1 + _buf[i + 1] + 1 + 1;
                }
                else if(_buf[i] >= Constant::REC_LEN_1 &&
                        _buf[i] <= Constant::REC_LEN_15)
                {
                    ++i; //特殊
                }
                else
                {
                    i = 1 + ((_buf[i] & 0x03) + 1);
                }
            }
        }

        return doCmd(Constant::WR_DATA, _buf.data(), _pos);
    }

    bool delBatch(boost::uint8_t buffID)
    {
        _shared->isDoNow = true;

        _pos = 0;
        return true;

    }

    bool runBatch(boost::uint8_t *buffID, std::size_t count, bool isExecuteMany)
    {
        boost::uint8_t cmd;

        if (buffID[0] == Constant::LINKBLOCK)
        {
            if (isExecuteMany)
            {
                cmd = Constant::DO_BAT_LN;
            }
            else
            {
                cmd = Constant::DO_BAT_L;
            }
        }
        else
        {
            if (isExecuteMany)
            {
                cmd = Constant::DO_BAT_CN;
            }
            else
            {
                cmd = Constant::DO_BAT_C;
            }
        }
        return doCmd(cmd, NULL, 0);
    }

    bool checkResult(boost::int64_t microseconds)
    {
        boost::uint8_t rb = 0;

        boost::system::error_code ec;
        stream()->setReadTimeout(MicroSeconds(microseconds));

        if (stream()->read(MutableBuffer(&rb, 1), ec) != 1)
        {
            return false;
        }

        if (rb == Constant::READY || rb == Constant::ERR)
        {
            stream()->discardInBuffer();
            return true;
        }

        return false;
    }
private:
    bool checkIdle()
    {
        boost::uint8_t rb = Constant::READY;
        boost::int32_t avail;

        avail = stream()->bytesAvailable();

        if (avail > 20)
        {
            stream()->discardInBuffer();
            return true;
        }
        boost::system::error_code ec;

        while (stream()->bytesAvailable())
        {
            stream()->read(MutableBuffer(&rb, 1), ec);
        }

        if (rb == Constant::READY || rb == Constant::ERR)
        {
            return true;
        }

        if (stream()->read(MutableBuffer(&rb, 1), ec) != 1)
        {
            return false;
        }

        if (rb == Constant::READY || rb == Constant::ERR)
        {
            return true;
        }

        return false;
    }

    bool checkSend()
    {
        boost::system::error_code ec;
        boost::uint8_t rb = 0;
        if (stream()->read(MutableBuffer(&rb, 1), ec) != 1)
        {
            return false;
        }

        if (rb == Constant::RECV_OK)
        {
            return true;
        }

        return false;
    }

    bool sendCmd(boost::uint8_t cmd, const boost::uint8_t *data, std::size_t count)
    {
        boost::scoped_array<boost::uint8_t> command(new boost::uint8_t[count + 2]);
        std::size_t i;
        boost::uint8_t cs = cmd;

        cmd += _runFlag;

        command[0] = cmd;

        for (i = 0; i < count; ++i)
        {
            command[i + 1] = data[i];
            cs += data[i];
        }

        command[i + 1] = cs;

        boost::system::error_code ec;
        for (i = 0; i < 3; ++i)
        {
            if (!checkIdle())
            {
                continue;
            }

            if (stream()->write(ConstBuffer(command.get(), count + 2), ec) !=
                    count + 2)
            {
                continue;
            }

            if (checkSend())
            {
                return true;
            }
        }
        return false;
    }

    bool doCmd(boost::uint8_t cmd, const boost::uint8_t *buff, std::size_t count)
    {
        bool ret = false;

        _startPos = 0;

        if (cmd != Constant::WR_DATA && cmd != Constant::SEND_DATA)
        {
            cmd |= count; //加上长度位
        }

        if (_shared->isDoNow)
        {
            //发送到BOX执行
            if (cmd == Constant::WR_DATA)
            {
                if (count == 0)
                {
                    return false;
                }

                if (_isLink)
                {
                    cmd = 0xFF; //写链路保持
                }
                else
                {
                    cmd = 0x00; //写通讯命令
                }
                boost::scoped_array<boost::uint8_t> temp(new boost::uint8_t[count + 2]);
                temp[0] = cmd;
                temp[1] = count;
                memcpy(temp.get() + 2, buff, count);
                ret = sendCmd(Constant::WR_DATA, temp.get(), count + 2);
                return ret;
            }
            else if (cmd == Constant::SEND_DATA)
            {
                if (count == 0)
                    return false;
                boost::scoped_array<boost::uint8_t> temp(new boost::uint8_t[count + 4]);
                temp[0] = 0x00; //写入位置
                temp[1] = count + 2; //数据包长度
                temp[2] = Constant::SEND_DATA; //命令
                temp[3] = count - 1; //命令长度-1
                memcpy(temp.get() + 4, buff, count);
                ret = sendCmd(Constant::WR_DATA, temp.get(), count + 4);
                if (!ret)
                {
                    return false;
                }
                return sendCmd(Constant::DO_BAT_C, NULL, 0);
            }
            else
            {
                return sendCmd(cmd, buff, count);
            }
        }
        else
        {
            //写命令到缓冲区
            _buf[_pos++] = cmd;
            if (cmd == Constant::SEND_DATA)
            {
                _buf[_pos++] = count - 1;
            }
            _startPos = _pos;
            if (count > 0)
            {
                memcpy(_buf.data() + _pos, buff, count);
            }
            _pos += count;
            return true;
        }
        return false;
    }

    bool doSet(boost::uint8_t cmd, const boost::uint8_t *buff, std::size_t count)
    {
        bool result = false;

        result = doCmd(cmd, buff, count);
        if (result && _shared->isDoNow)
        {
            result = checkResult(toMicroSeconds(MilliSeconds(150)));
        }
        return result;
    }

    boost::uint8_t getBuffData(boost::uint8_t addr, boost::uint8_t *buff, std::size_t count)
    {
        boost::array<boost::uint8_t, 2> temp;
        temp[0] = addr;
        temp[1] = count;

        if (!doCmd(Constant::GET_BUF, temp.data(), 2))
        {
            return 0;
        }

        return getCmdData(buff, count);
    }

    bool initBox()
    {
        static boost::uint8_t password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 0x17, 0x2D, 0x4D};
        boost::array<boost::uint8_t, 32> buf;
        std::size_t i;
        boost::uint8_t run = 0;

        srand((boost::uint8_t)time(NULL));
        _shared->isDoNow = true;
        _runFlag = 0;

        for (i = 0; i < 4; ++i)
        {
            buf[i] = LowByte(rand());
        }

        for (i = 0; i < 10; ++i)
        {
            run += password[i] ^ buf[i % 3 + 1];
        }

        if (run == 0)
        {
            run = 0x55;
        }

        if (!doCmd(Constant::GET_CPU, buf.data() + 1, 3))
        {
            return false;
        }
        if (getCmdData(buf.data(), 32) <= 0)
        {
            return false;
        }
        _runFlag = 0; // Run
        _boxTimeUnit = 0;
        for (i = 0; i < 3; ++i)
        {
            _boxTimeUnit = (_boxTimeUnit << 8) | buf[i];
        }
        _timeBaseDB = buf[i++];
        _timeExternDB = buf[i++];

        for (i = 0; i < Constant::MAXPORT_NUM; ++i)
        {
            _ports[i] = 0xFF;
        }

        _pos = 0;
        _shared->isDB20 = false;
        return true;
    }

    boost::uint8_t getCmdData(boost::uint8_t *receiveBuffer, std::size_t count)
    {
        boost::uint8_t len = 0;
        boost::uint8_t cs = 0;

        if ((readBytes(receiveBuffer, 1) != 1) ||
                (readBytes(&len, 1) != 1))
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

    bool checkBox()
    {
        boost::array<boost::uint8_t, 32> buf;

        if (!doCmd(Constant::GET_BOXID, NULL, 0))
        {
            return false;
        }

        if (getCmdData(buf.data(), buf.size()) <= 0)
        {
            return false;
        }

        _boxVer = (buf[10] << 8) | buf[11];
        return true;
    }

    bool setRF(boost::uint8_t cmd, boost::uint8_t data)
    {
        return false;
    }

    bool openBox()
    {
        if (initBox() && checkBox())
        {
            stream()->discardInBuffer();
            stream()->discardOutBuffer();
            return true;
        }
        return false;
    }

    bool reset()
    {
        std::size_t i;

        stopNow(true);
        stream()->discardOutBuffer();
        stream()->discardInBuffer();

        for (i = 0; i < Constant::MAXPORT_NUM; ++i)
        {
            _ports[i] = 0xFF;
        }

        return doCmd(Constant::RESET, NULL, 0);
    }

    bool copyBuff(boost::uint8_t dest, boost::uint8_t src, boost::uint8_t len)
    {
        boost::array<boost::uint8_t, 3> buf;

        buf[0] = dest;
        buf[1] = src;
        buf[2] = len;

        return doSet(Constant::COPY_BYTE, buf.data(), buf.size());
    }

    boost::uint32_t getBoxVer()
    {
        return _boxVer;
    }

private:
    boost::uint32_t _boxTimeUnit;
    boost::uint32_t _timeBaseDB;
    boost::uint32_t _timeExternDB;
    boost::uint32_t _pos;
    bool _isLink;
    boost::uint32_t _runFlag;
    boost::uint32_t _boxVer;
    boost::uint32_t _startPos;
    boost::array<boost::uint8_t, Constant::MAXPORT_NUM> _ports;
    boost::array<boost::uint8_t, Constant::MAXBUFF_LEN> _buf;
};
}
}
}
}

#endif