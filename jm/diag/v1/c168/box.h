#ifndef __JM_DIAG_v1_C168_H__
#define __JM_DIAG_v1_C168_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/v1/box.h>
#include <jm/diag/v1/c168/constant.h>
#include <jm/utils.h>

namespace JM
{
namespace Diag
{
namespace V1
{
namespace C168
{

class Box : public V1::Box<C168::Constant>
{
public:

    Box(const boost::shared_ptr<V1::Shared> &shared)
    : V1::Box<C168::Constant>(shared)
    , _buffUsedNum(0)
    , _timeUnit(0)
    , _timeBaseDB(0)
    , _timeExternDB(0)
    , _cmdBuffLen(0)
    , _buffID(0)
    , _headPassword(0)
    {
        memset(_buffAdd.data(), 0, _buffAdd.size());
        memset(_buffUsed.data(), 0, _buffUsed.size());
        memset(_version.data(), 0, _version.size());
        memset(_boxID.data(), 0, _boxID.size());
        memset(_port.data(), 0, _port.size());
        memset(_cmdTemp.data(), 0, _cmdTemp.size());
    }

    bool open_comm()
    {
        if (stream().type() == BoxStream::SerialPort)
        {
            JM::Asio::serial_port_ptr port = stream().serial_port();
            StringVector vec = JM::Asio::serial_port::get_system_ports();

            boost::system::error_code ec;
            for (StringVector::iterator it = vec.begin(); it != vec.end(); it++)
            {
                port->open(*it, ec);
                if (!ec)
                {
                    port->set_option(JM::Asio::serial_port::baud_rate(9600), ec);
                    port->set_option(JM::Asio::serial_port::parity(), ec);
                    port->set_option(JM::Asio::serial_port::stop_bits(), ec);
                    port->set_option(JM::Asio::serial_port::flow_control(), ec);
                    port->set_option(JM::Asio::serial_port::character_size(), ec);
                    port->set_dtr(true, ec);
                    if (open_box())
                        return true;
                }
                port->close(ec);
            }
        }
        return false;
    }

    bool close_comm()
    {
        boost::system::error_code ec;
        if (stream().type() == BoxStream::SerialPort)
        {
            JM::Asio::serial_port_ptr port = stream().serial_port();
            if (!port->is_open())
                return true;
        }

        stop_now(true);
        do_set(Constant::RESET, NULL, 0);
        set_rf(Constant::RESET_RF, 0);
        if (stream().type() == BoxStream::SerialPort)
        {
            stream().serial_port()->close(ec);
        }

        return true;
    }

    bool del_batch(boost::uint8_t buffID)
    {
        if (buffID > Constant::MAXIM_BLOCK)
        {
            // 数据块不存在
            _shared->lastError = Constant::NODEFINE_BUFF;
            return false;
        }

        if (_buffID == buffID)
        {
            _buffID = Constant::NULLADD;
            return true;
        }

        if (_buffAdd[buffID] == Constant::NULLADD)
        {
            // 数据块标识登记是否有申请?
            _shared->lastError = Constant::NOUSED_BUFF;
            return false;
        }

        if (buffID == Constant::LINKBLOCK)
        {
            _buffAdd[Constant::LINKBLOCK] = _cmdBuffLen;
        }
        else
        {
            std::size_t i = 0;
            boost::array<boost::uint8_t, 3 > data;
            boost::uint8_t deleteBuffLen;

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
                data[2] = _buffAdd[Constant::SWAPBLOCK] - data[1];

                if (!do_set(Constant::COPY_DATA - Constant::COPY_DATA % 4, data.data(), data.size()))
                {
                    return false;
                }
            }
            else
            {
                data[1] = _buffAdd[Constant::SWAPBLOCK];
            }

            deleteBuffLen = data[1] - data[0];
            for (i = i + 1; i < _buffUsedNum; i++)
            {
                _buffUsed[i - 1] = _buffUsed[i];
                _buffAdd[_buffUsed[i]] = _buffAdd[_buffUsed[i]] - deleteBuffLen;
            }
            _buffUsedNum--;
            _buffAdd[Constant::SWAPBLOCK] = _buffAdd[Constant::SWAPBLOCK] - deleteBuffLen;
            _buffAdd[_buffID] = Constant::NULLADD;
        }
        return true;
    }

    bool new_batch(boost::uint8_t buffID)
    {
        if (buffID > Constant::MAXIM_BLOCK)
        {
            _shared->lastError = Constant::NODEFINE_BUFF;
            return false;
        }
        if (_buffID != Constant::NULLADD)
        {
            _shared->lastError = Constant::APPLICATION_NOW;
            return false;
        }

        if ((_buffAdd[buffID] != Constant::NULLADD &&
                (buffID != Constant::LINKBLOCK) &&
                !del_batch(buffID)))
        {
            return false;
            ;
        }

        _cmdTemp[0] = Constant::WR_DATA;
        _cmdTemp[1] = 0x01;
        if (_buffID == Constant::LINKBLOCK)
        {
            _cmdTemp[2] = 0xFF;
            _buffAdd[Constant::LINKBLOCK] = _cmdBuffLen;
        }
        else
        {
            _cmdTemp[2] = _buffAdd[Constant::SWAPBLOCK];
        }

        if (bufferSize() <= 1)
        {
            _shared->lastError = Constant::BUFFFLOW;
            return false;
        }
        _cmdTemp[3] = Constant::WR_DATA + 0x01 + _cmdTemp[2];
        _cmdTemp[0] += _headPassword;
        _buffID = buffID;
        _shared->isDoNow = false;
        return true;
    }

    bool add_to_buff(boost::uint8_t commandWord, const boost::uint8_t *data, std::size_t count)
    {
        std::size_t i;
        boost::uint8_t checksum;

        checksum = _cmdTemp[_cmdTemp[1] + 2];

        _shared->nextAddress = _cmdTemp[1] + count + 1;

        if (_buffID == Constant::NULLADD)
        {
            // 数据块标识登记是否有申请?
            _shared->lastError = Constant::NOAPPLICATBUFF;
            _shared->isDoNow = true;
            return false;
        }

        if (bufferSize() < _shared->nextAddress)
        {
            // 检查是否有足够的空间存储?
            _shared->lastError = Constant::BUFFFLOW;
            _shared->isDoNow = true;
            return false;
        }

        if (commandWord < Constant::RESET &&
                commandWord != Constant::CLR_LINK &&
                commandWord != Constant::DO_BAT_00 &&
                commandWord != Constant::D0_BAT &&
                commandWord != Constant::D0_BAT_FOR &&
                commandWord != Constant::WR_DATA)
        {
            // 是否为缓冲区命令?
            if ((count < Constant::CMD_DATALEN) ||
                    (commandWord == Constant::SEND_DATA &&
                    count < Constant::SEND_LEN))
            {
                // 是否合法命令?
                if (commandWord == Constant::SEND_DATA &&
                        getboxVer() > 0x400)
                {
                    // 增加发送长命令
                    _cmdTemp[_cmdTemp[1] + 2] = Constant::SEND_CMD;
                    checksum += Constant::SEND_CMD;
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
                return true;
            }
            _shared->lastError = Constant::ILLIGICAL_LEN;
            _shared->isDoNow = true;
            return false;
        }
        _shared->lastError = Constant::UNBUFF_CMD;
        _shared->isDoNow = true;
        return false;
    }

    bool end_batch()
    {
        boost::system::error_code ec;
        boost::uint32_t times = Constant::REPLAYTIMES;

        _shared->isDoNow = true;

        if (_buffID == Constant::NULLADD)
        {
            // 数据块标识登记是否有申请?
            _shared->lastError = Constant::NOAPPLICATBUFF;
            return false;
        }

        if (_cmdTemp[1] == 0x01)
        {
            _buffID = Constant::NULLADD;
            _shared->lastError = Constant::NOADDDATA;
            return false;
        }

        while (times--)
        {
            if (times == 0)
            {
                _buffID = Constant::NULLADD;
                return false;
            }
            if (!check_idle() ||
                    (stream().vs()->write(ConstBuffer(_cmdTemp.data(), _cmdTemp[1] + 3), ec) != (_cmdTemp[1] + 3)))
            {
                continue;
            }
            else if (send_ok(MilliSeconds(20 * (_cmdTemp[1] + 10)).total_microseconds()))
            {
                break;
            }
            if (!stop_now(true))
            {
                _buffID = Constant::NULLADD;
                return false;
            }
        }

        if (_buffID == Constant::LINKBLOCK)
        {
            _buffAdd[Constant::LINKBLOCK] = _cmdBuffLen - _cmdTemp[1];
        }
        else
        {
            _buffAdd[_buffID] = _buffAdd[Constant::SWAPBLOCK];
            _buffUsed[_buffUsedNum] = _buffID;
            _buffUsedNum++;
            _buffAdd[Constant::SWAPBLOCK] = _buffAdd[Constant::SWAPBLOCK] + _cmdTemp[1];
        }
        _buffID = Constant::NULLADD;
        return true;
    }

    bool send_to_box(boost::uint8_t commandWord, const boost::uint8_t *buff, std::size_t count)
    {
        if (_shared->isDoNow)
        {
            return do_set(commandWord, buff, count);
        }
        else
        {
            return add_to_buff(commandWord, buff, count);
        }
    }

    bool set_line_level(boost::uint8_t valueLow, boost::uint8_t valueHigh)
    {
        // 只有一个字节的数据，设定端口1
        _port[1] &= ~valueLow;
        _port[1] |= valueHigh;
        return send_to_box(Constant::SETPORT1, _port.data() + 1, 1);
    }

    bool set_comm_ctrl(boost::uint8_t valueOpen, boost::uint8_t valueClose)
    {
        // 只有一个字节的数据，设定端口2
        _port[2] &= ~valueOpen;
        _port[2] |= valueClose;
        return send_to_box(Constant::SETPORT2, _port.data() + 2, 1);
    }

    bool set_comm_line(boost::uint8_t sendLine, boost::uint8_t recvLine)
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
        return send_to_box(Constant::SETPORT0, _port.data(), 1);
    }

    bool turn_over_one_by_one()
    {
        // 将原有的接受一个发送一个的标志翻转
        return send_to_box(Constant::SET_ONEBYONE, NULL, 0);
    }

    bool set_echo_data(boost::uint8_t *echoBuff, std::size_t echoLen)
    {
        if (echoBuff == NULL || echoLen == 0 || echoLen > 4)
        {
            _shared->lastError = Constant::ILLIGICAL_LEN;
            return false;
        }
        if (_shared->isDoNow)
        {
            boost::array<boost::uint8_t, 6 > receiveBuff;
            std::size_t i;
            if (!commbox_do(Constant::Echo, echoBuff, echoLen) ||
                    (read_data(receiveBuff.data(), echoLen, MilliSeconds(100).total_microseconds()) != echoLen))
            {
                return false;
            }
            for (i = 0; i < echoLen; i++)
            {
                if (receiveBuff[i] != echoBuff[i])
                {
                    _shared->lastError = Constant::CHECKSUM_ERROR;
                    return false;
                }
            }
            return check_result(MilliSeconds(100).total_microseconds());
        }
        return add_to_buff(Constant::Echo, echoBuff, echoLen);
    }

    bool keep_link(bool isRunLink)
    {
        if (isRunLink)
        {
            return send_to_box(Constant::RUNLINK, NULL, 0);
        }
        return send_to_box(Constant::STOPLINK, NULL, 0);
    }

    bool set_comm_link(boost::uint8_t ctrlWord1, boost::uint8_t ctrlWord2, boost::uint8_t ctrlWord3)
    {
        boost::array<boost::uint8_t, 3 > ctrlWord; // 通讯控制字3
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
            return send_to_box(Constant::SETTING, ctrlWord.data(), 1);
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
        if (modeControl == Constant::EXRS_232 && length < 2)
        {
            _shared->lastError = Constant::UNSET_EXRSBIT;
            return false;
        }

        return send_to_box(Constant::SETTING, ctrlWord.data(), length);
    }

    bool set_comm_baud(boost::uint32_t baud)
    {
        boost::array<boost::uint8_t, 2 > baudTime;
        double instructNum;

        instructNum = ((1000000.0 / (_timeUnit)) * 1000000) / baud;

        if (_shared->isDB20)
        {
            instructNum /= 20;
        }

        instructNum += 0.5;
        if (instructNum > 65535 || instructNum < 10)
        {
            _shared->lastError = Constant::COMMBAUD_OUT;
            return false;
        }
        baudTime[0] = high_byte(instructNum);
        baudTime[1] = low_byte(instructNum);
        if (baudTime[0] == 0)
        {
            return send_to_box(Constant::SETBAUD, baudTime.data() + 1, 1);
        }
        else
        {
            return send_to_box(Constant::SETBAUD, baudTime.data(), 2);
        }
    }

    bool set_comm_time(boost::uint8_t type, boost::int64_t time)
    {
        boost::array<boost::uint8_t, 2 > timeBuff;

        get_link_time(type, time);

        if (type == Constant::SETVPWSTART ||
                type == Constant::SETVPWRECS)
        {
            if (Constant::SETVPWRECS == type)
            {
                time = (time * 2) / 3;
            }
            type = type + (Constant::SETBYTETIME & 0xF0);
            time = time / ((_timeUnit / 1000000.0));
        }
        else
        {
            time = ((time / _timeBaseDB) / (_timeUnit / 1000000.0));
        }

        if (time > 65535)
        {
            _shared->lastError = Constant::COMMTIME_OUT;
            return false;
        }

        if (type == Constant::SETBYTETIME ||
                type == Constant::SETWAITTIME ||
                type == Constant::SETRECBBOUT ||
                type == Constant::SETRECFROUT ||
                type == Constant::SETLINKTIME)
        {
            timeBuff[0] = high_byte(time);
            timeBuff[1] = low_byte(time);
            if (timeBuff[0] == 0)
            {
                return send_to_box(type, timeBuff.data() + 1, 1);
            }
            return send_to_box(type, timeBuff.data(), 2);
        }
        _shared->lastError = Constant::UNDEFINE_CMD;
        return false;
    }

    bool run_receive(boost::uint8_t type)
    {
        if (type == Constant::GET_PORT1)
        {
            _shared->isDB20 = false;
        }

        if (type == Constant::GET_PORT1 ||
                type == Constant::SET55_BAUD ||
                (type >= Constant::REC_FR && type <= Constant::RECEIVE))
        {
            if (_shared->isDoNow)
            {
                return commbox_do(type, NULL, 0);
            }
            return add_to_buff(type, NULL, 0);
        }
        _shared->lastError = Constant::UNDEFINE_CMD;
        return false;
    }

    boost::uint8_t get_abs_add(boost::uint8_t _buffID, boost::uint8_t add)
    {
        std::size_t length = 0;
        boost::uint8_t start_add = 0;

        if (_buffID != _buffID)
        {
            if (_buffAdd[_buffID] == Constant::NULLADD)
            {
                _shared->lastError = Constant::NOUSED_BUFF;
                return 0;
            }

            if (_buffID == Constant::LINKBLOCK)
            {
                length = _cmdBuffLen - _buffAdd[Constant::LINKBLOCK];
            }
            else
            {
                std::size_t i;
                for (i = 0; i < _buffUsedNum; i++)
                {
                    if (_buffUsed[i] == _buffID)
                    {
                        break;
                    }
                }
                if (i == (_buffUsedNum - 1))
                {
                    length = _buffAdd[Constant::SWAPBLOCK] - _buffAdd[_buffID];
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
            length = _buffAdd[Constant::LINKBLOCK] - _buffAdd[Constant::SWAPBLOCK];
            start_add = _buffAdd[Constant::SWAPBLOCK];
        }

        if (add < length)
        {
            return add + start_add;
        }

        _shared->lastError = Constant::OUTADDINBUFF;
        return 0;
    }

    bool update_buff(boost::uint8_t type, boost::uint8_t *buffer)
    {
        boost::array<boost::uint8_t, 4 > cmdTemp;
        boost::uint8_t ret;

        _shared->lastError = 0;
        ret = get_abs_add(buffer[0], buffer[1]);
        if (ret == 0)
        {
            return false;
        }
        cmdTemp[0] = ret;

        if ((type == Constant::INVERT_DATA) ||
                (type == Constant::DEC_DATA) ||
                (type == Constant::INC_DATA))
        {
        }
        else if ((type == Constant::UPDATE_1BYTE) ||
                (type == Constant::SUB_BYTE))
        {
            cmdTemp[1] = buffer[2];
        }
        else if (type == Constant::INC_2DATA)
        {
            ret = get_abs_add(buffer[2], buffer[3]);
            if (ret == 0)
            {
                return false;
            }
            cmdTemp[1] = ret;
        }
        else if ((type == Constant::COPY_DATA) ||
                (type == Constant::ADD_1BYTE))
        {
            ret = get_abs_add(buffer[2], buffer[3]);
            if (ret == 0)
            {
                return false;
            }
            cmdTemp[1] = ret;
            cmdTemp[2] = buffer[4];
        }
        else if ((type == Constant::UPDATE_2BYTE) ||
                (type == Constant::ADD_2BYTE))
        {
            ret = get_abs_add(buffer[3], buffer[4]);
            if (ret == 0)
            {
                return false;
            }
            cmdTemp[1] = buffer[2];
            cmdTemp[2] = ret;
            cmdTemp[3] = buffer[5];

        }
        else if ((type == Constant::ADD_DATA) ||
                (type == Constant::SUB_DATA))
        {
            ret = get_abs_add(buffer[2], buffer[3]);
            if (ret == 0)
            {
                return false;
            }
            cmdTemp[1] = ret;
            ret = get_abs_add(buffer[4], buffer[5]);
            if (ret == 0)
            {
                return false;
            }
            cmdTemp[2] = ret;

        }
        else
        {
            _shared->lastError = Constant::UNDEFINE_CMD;
            return false;

        }
        return send_to_box(type - type % 4, cmdTemp.data(), type % 4 + 1);
    }

    bool commbox_delay(boost::int64_t time)
    {
        boost::array<boost::uint8_t, 2 > timeBuff;
        boost::uint8_t delayWord = Constant::DELAYSHORT;

        time = time / (_timeUnit / 1000000.0);
        if (time == 0)
        {
            _shared->lastError = Constant::SETTIME_ERROR;
            return false;
        }
        if (time > 65535)
        {
            time = time / _timeBaseDB;
            if (time > 65535)
            {
                time = time / _timeBaseDB;
                if (time > 65535)
                {
                    _shared->lastError = Constant::COMMTIME_OUT;
                    return false;
                }
                delayWord = Constant::DELAYLONG;
            }
            else
            {
                delayWord = Constant::DELAYTIME;
            }
        }
        timeBuff[0] = high_byte(time);
        timeBuff[1] = low_byte(time);
        if (timeBuff[0] == 0)
        {
            if (_shared->isDoNow)
            {
                return commbox_do(delayWord, timeBuff.data() + 1, 1);
            }
            return add_to_buff(delayWord, timeBuff.data() + 1, 1);
        }
        if (_shared->isDoNow)
        {
            return commbox_do(delayWord, timeBuff.data(), 2);
        }
        return add_to_buff(delayWord, timeBuff.data(), 2);
    }

    bool send_out_data(const boost::uint8_t *buffer, std::size_t count)
    {
        if (count == 0)
        {
            _shared->lastError = Constant::ILLIGICAL_LEN;
            return false;
        }
        if (_shared->isDoNow)
        {
            return commbox_do(Constant::SEND_DATA, buffer, count);
        }
        else
        {
            return add_to_buff(Constant::SEND_DATA, buffer, count);
        }
    }

    bool run_batch(boost::uint8_t *buffID, std::size_t count, bool isExecuteMany)
    {
        boost::uint8_t commandWord = Constant::D0_BAT;
        std::size_t i;

        for (i = 0; i < count; i++)
        {
            if (_buffAdd[buffID[i]] == Constant::NULLADD)
            {
                _shared->lastError = Constant::NOUSED_BUFF;
                return false;
            }
        }
        if (isExecuteMany)
        {
            commandWord = Constant::D0_BAT_FOR;
        }
        if (commandWord == Constant::D0_BAT &&
                buffID[0] == _buffUsed[0])
        {
            commandWord = Constant::DO_BAT_00;
            return commbox_do(commandWord, NULL, 0);
        }

        return commbox_do(commandWord, buffID, count);
    }

    std::size_t read_data(boost::uint8_t *buff, std::size_t count, boost::int64_t microseconds)
    {
        boost::system::error_code ec;
        stream().vs()->set_read_timeout(MicroSeconds(microseconds));

        return stream().vs()->read(MutableBuffer(buff, count), ec);
    }

    std::size_t read_bytes(boost::uint8_t *buff, std::size_t count)
    {
        return read_data(buff, count, _shared->resWaitTime);
    }

    bool check_result(boost::int64_t microseconds)
    {
        boost::system::error_code ec;
        boost::uint8_t receiveBuffer;

        stream().vs()->set_read_timeout(MicroSeconds(microseconds));

        if (stream().vs()->read(MutableBuffer(&receiveBuffer, 1), ec) != 1)
        {
            _shared->lastError = Constant::TIMEOUT_ERROR;
        }

        if (receiveBuffer == Constant::SUCCESS)
        {
            return true;
        }

        while (stream().vs()->read(MutableBuffer(&receiveBuffer, 1), ec) == 1);
        _shared->lastError = receiveBuffer;
        return false;
    }

    bool stop_now(bool isStopExecute)
    {
        boost::system::error_code ec;
        if (isStopExecute)
        {
            boost::uint8_t receiveBuffer = 0;
            boost::uint32_t times = Constant::REPLAYTIMES;
            while (times--)
            {
                if (!commbox_do(Constant::STOP_EXECUTE, NULL, 0))
                {
                    return false;
                }
                if (stream().vs()->read(MutableBuffer(&receiveBuffer, 1), ec) != 1)
                {
                    _shared->lastError = Constant::TIMEOUT_ERROR;
                }
                if (receiveBuffer == Constant::RUN_ERR)
                {
                    return true;
                }
                _shared->lastError = Constant::TIMEOUT_ERROR;
            }
            return false;
        }
        return commbox_do(Constant::STOP_REC, NULL, 0);
    }
private:

    bool check_idle()
    {
        boost::system::error_code ec;
        boost::uint8_t receiveBuffer = Constant::SUCCESS;
        std::size_t avail;

        avail = stream().vs()->bytes_available();

        if (avail > 240)
        {
            stream().vs()->discard_in_buffer();
            stream().vs()->discard_out_buffer();
            return true;
        }

        while (avail)
        {
            stream().vs()->read(MutableBuffer(&receiveBuffer, 1), ec);
            avail--;
        }

        if (receiveBuffer == Constant::SUCCESS)
        {
            return true;
        }

        stream().vs()->set_read_timeout(MilliSeconds(200));

        if (stream().vs()->read(MutableBuffer(&receiveBuffer, 1), ec) != 1)
        {
            _shared->lastError = Constant::KEEPLINK_ERROR;
            return false;
        }

        if (receiveBuffer == Constant::SUCCESS)
        {
            return true;
        }

        _shared->lastError = receiveBuffer;
        return false;
    }

    bool send_ok(boost::int64_t microseconds)
    {
        boost::system::error_code ec;
        boost::uint8_t receiveBuffer = 0;

        stream().vs()->set_read_timeout(MicroSeconds(microseconds));

        while (true)
        {
            if (stream().vs()->read(MutableBuffer(&receiveBuffer, 1), ec) != 1)
            {
                _shared->lastError = Constant::TIMEOUT_ERROR;
                return false;
            }

            if (receiveBuffer == Constant::SEND_OK)
            {
                return true;
            }
            else if (receiveBuffer >= Constant::UP_TIMEOUT &&
                    receiveBuffer <= Constant::ERROR_REC)
            {
                _shared->lastError = Constant::SENDDATA_ERROR;
                return false;
            }
        }

        _shared->lastError = Constant::SENDDATA_ERROR;
        return false;
    }

    boost::uint32_t getboxVer()
    {
        return (_version[0] << 8) | (_version[1]);
    }

    std::size_t bufferSize()
    {
        return _buffAdd[Constant::LINKBLOCK] - _buffAdd[Constant::SWAPBLOCK];
    }

    bool sendDataToEcuNew(const boost::uint8_t *buff, std::size_t count)
    {
        std::size_t i;
        std::size_t checksum = count + 5;
        std::size_t size = count + 6;
        boost::scoped_array<boost::uint8_t> command(new boost::uint8_t[size]);

        command[0] = Constant::WR_DATA + _headPassword;
        command[1] = low_byte(count + 3);
        command[2] = _buffAdd[Constant::SWAPBLOCK];
        command[3] = Constant::SEND_CMD;
        command[4] = low_byte(count - 1);

        command[checksum] = Constant::WR_DATA + command[1] + command[2] + command[3] + command[4];

        memcpy(command.get() + 5, buff, count);

        for (i = 0; i < count; i++)
        {
            command[checksum] += buff[i];
        }

        boost::system::error_code ec;
        for (i = 0; i < 3; i++)
        {
            if (!check_idle() ||
                    (stream().vs()->write(ConstBuffer(command.get(), size), ec) != size))
            {
                _shared->lastError = Constant::SENDDATA_ERROR;
                continue;
            }

            if (send_ok(MilliSeconds(20 * (count + 12)).total_microseconds()))
            {
                return true;
            }
        }
        return false;
    }

    bool sendDataToEcuOld(const boost::uint8_t *buff, std::size_t count)
    {
        std::size_t i;
        std::size_t checksum = count + 4;
        std::size_t size = count + 5;
        boost::scoped_array<boost::uint8_t> command(new boost::uint8_t[size]);

        command[0] = Constant::WR_DATA + _headPassword;
        command[1] = low_byte(count + 2);
        command[2] = _buffAdd[Constant::SWAPBLOCK];
        command[3] = low_byte(count - 1);

        command[checksum] = Constant::WR_DATA + command[1] + command[2] + command[3];

        memcpy(command.get() + 4, buff, count);

        for (i = 0; i < count; i++)
        {
            command[checksum] += buff[i];
        }

        boost::system::error_code ec;
        for (i = 0; i < 3; i++)
        {
            if (!check_idle() ||
                    (stream().vs()->write(ConstBuffer(command.get(), size), ec) != size))
            {
                _shared->lastError = Constant::SENDDATA_ERROR;
                continue;
            }

            if (send_ok(MilliSeconds(20 * (size + 6)).total_microseconds()))
            {
                return true;
            }
        }
        return false;

    }

    bool sendDataToEcu(boost::uint8_t commandWord, const boost::uint8_t *buff, std::size_t count)
    {
        if (commandWord == Constant::SEND_DATA &&
                count <= Constant::SEND_LEN)
        {
            if (bufferSize() < (count + 1))
            {
                _shared->lastError = Constant::NOBUFF_TOSEND;
                return false;
            }

            if (getboxVer() > 0x400)
            {
                // 增加发送长命令
                if (!sendDataToEcuNew(buff, count))
                {
                    return false;
                }
            }
            else
            {
                // 保持与旧盒子兼容
                if (!sendDataToEcuOld(buff, count))
                {
                    return false;
                }
            }
            return commbox_do(Constant::D0_BAT, _buffAdd.data() + Constant::SWAPBLOCK, 1);
        }
        _shared->lastError = Constant::ILLIGICAL_LEN;
        return false;
    }

    bool commboxCommand(boost::uint8_t commandWord, const boost::uint8_t *buff, std::size_t count)
    {
        std::size_t size = count + 2;
        std::size_t csPos = count + 1;
        std::size_t i;
        boost::scoped_array<boost::uint8_t> command(new boost::uint8_t[size]);
        boost::uint8_t *checksum = &(command.get()[csPos]);

        *checksum = low_byte(commandWord + count);

        if (commandWord < Constant::WR_DATA)
        {
            if (count == 0)
            {
                _shared->lastError = Constant::ILLIGICAL_LEN;
                return false;
            }
            (*checksum)--;
        }
        else
        {
            if (count != 0)
            {
                _shared->lastError = Constant::ILLIGICAL_LEN;
                return false;
            }
        }

        command[0] = *checksum + _headPassword;
        memcpy(command.get() + 1, buff, count);

        for (i = 0; i < count; i++)
        {
            *checksum += buff[i];
        }

        boost::system::error_code ec;
        for (i = 0; i < 3; i++)
        {
            if (commandWord != Constant::STOP_REC &&
                    commandWord != Constant::STOP_EXECUTE)
            {
                if (!check_idle() ||
                        (stream().vs()->write(ConstBuffer(command.get(), size), ec) != size))
                {
                    _shared->lastError = Constant::SENDDATA_ERROR;
                    continue;
                }
            }
            else
            {
                if (stream().vs()->write(ConstBuffer(command.get(), size), ec) != size)
                {
                    _shared->lastError = Constant::SENDDATA_ERROR;
                    continue;
                }
            }

            if (send_ok(to_micro_seconds(MilliSeconds(200 * size))))
            {
                return true;
            }
        }
        return false;
    }

    bool commbox_do(boost::uint8_t commandWord, const boost::uint8_t *buff, std::size_t count)
    {
        if (count > Constant::CMD_DATALEN)
        {
            return sendDataToEcu(commandWord, buff, count);
        }
        else
        {
            return commboxCommand(commandWord, buff, count);
        }
    }

    bool do_set(boost::uint8_t commandWord, const boost::uint8_t *buff, std::size_t count)
    {
        boost::uint32_t times = Constant::REPLAYTIMES;

        while (times--)
        {
            if (!commbox_do(commandWord, buff, count))
            {
                continue;
            }
            if (check_result(to_micro_seconds(MilliSeconds(50))))
            {
                return true;
            }
            stop_now(true);
        }
        return false;
    }

    std::size_t get_cmd_data(boost::uint8_t command, boost::uint8_t *receiveBuffer)
    {
        boost::uint8_t checksum = command;
        boost::array<boost::uint8_t, 2 > cmdInfo;
        std::size_t i;

        if (read_data(cmdInfo.data(), cmdInfo.size(), to_micro_seconds(MilliSeconds(150))) != 2)
        {
            return 0;
        }

        if (cmdInfo[0] != command)
        {
            _shared->lastError = cmdInfo[0];
            stream().vs()->discard_in_buffer();
            return 0;
        }

        if ((read_data(receiveBuffer, cmdInfo[1], to_micro_seconds(MilliSeconds(150))) != cmdInfo[1]) ||
                (read_data(cmdInfo.data(), 1, to_micro_seconds(MilliSeconds(150))) != 1))
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
            _shared->lastError = Constant::CHECKSUM_ERROR;
            return 0;
        }
        return cmdInfo[1];
    }

    bool check_box()
    {
        static boost::uint8_t password[] = {0x0C, 0x22, 0x17, 0x41, 0x57, 0x2D, 0x43, 0x17, 0x2D, 0x4D};
        boost::array<boost::uint8_t, 5 > temp;
        std::size_t i;
        boost::uint8_t checksum;

        srand((boost::uint32_t)time(NULL));

        temp[4] = 0x00;
        i = 0;
        while (i < 4)
        {
            temp[i] = low_byte(rand());
            temp[4] += temp[i];
            i++;
        }

        boost::system::error_code ec;
        if (stream().vs()->write(ConstBuffer(temp.data(), temp.size()), ec) != 5)
        {
            _shared->lastError = Constant::SENDDATA_ERROR;
            return false;
        }

        i = 0;
        checksum = temp[4] + temp[4];

        while (i < sizeof (password))
        {
            checksum += password[i] ^ temp[i % 5];
            i++;
        }

        sleep(MilliSeconds(20));

        if (get_cmd_data(Constant::GETINFO, temp.data()) <= 0)
        {
            return false;
        }

        _headPassword = temp[0];

        if (checksum != _headPassword)
        {
            _shared->lastError = Constant::CHECKSUM_ERROR;
            return false;
        }
        if (_headPassword == 0)
        {
            _headPassword = 0x55;
        }
        return true;
    }

    bool init_box()
    {
        boost::uint32_t length;
        std::size_t pos = 0;
        std::size_t i;

        _headPassword = 0x00;
        _shared->isDB20 = false;

        if (!commbox_do(Constant::GETINFO, NULL, 0))
        {
            return false;
        }

        length = get_cmd_data(Constant::GETINFO, _cmdTemp.data());
        if (length < Constant::COMMBOXIDLEN)
        {
            _shared->lastError = Constant::LOST_VERSIONDATA;
            return false;
        }

        _timeUnit = 0;
        for (i = 0; i < Constant::MINITIMELEN; i++)
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
            _shared->lastError = Constant::COMMTIME_ZERO;
            return false;
        }

        for (i = 0; i < Constant::COMMBOXIDLEN; i++)
        {
            _boxID[i] = _cmdTemp[pos++];
        }

        for (i = 0; i < Constant::VERSIONLEN; i++)
        {
            _version[i] = _cmdTemp[pos++];
        }

        _port[0] = Constant::NULLADD;
        _port[1] = Constant::NULLADD;
        _port[2] = Constant::NULLADD;
        _port[3] = Constant::NULLADD;

        _buffID = Constant::NULLADD;
        _buffUsedNum = 0;

        memset(_buffAdd.data(), Constant::NULLADD, Constant::MAXIM_BLOCK);
        _buffAdd[Constant::LINKBLOCK] = _cmdBuffLen;
        _buffAdd[Constant::SWAPBLOCK] = 0;
        return true;
    }

    bool set_rf(boost::uint8_t cmd, boost::uint8_t cmdInfo)
    {
        std::size_t times = Constant::REPLAYTIMES;

        cmdInfo += cmd;
        if (cmd == Constant::SETRFBAUD)
        {
            times = 2;
        }

        sleep(MilliSeconds(6));

        boost::system::error_code ec;
        while (times--)
        {
            if (check_idle() &&
                    (stream().vs()->write(ConstBuffer(&cmdInfo, 1), ec) == 1))
            {
                if (!send_ok(to_micro_seconds(MilliSeconds(500))))
                {
                    continue;
                }

                if ((stream().vs()->write(ConstBuffer(&cmdInfo, 1), ec) != 1) ||
                        !check_result(to_micro_seconds(MilliSeconds(500))))
                {
                    continue;
                }
                sleep(MilliSeconds(500));
                return true;
            }
        }
        return false;
    }

    bool do_set_pc_baud(boost::uint8_t baud)
    {
        JM::Asio::serial_port_ptr port = stream().serial_port();

        if (!commbox_do(Constant::SET_UPBAUD, &baud, 1))
        {
            return false;
        }

        sleep(MilliSeconds(500));
        check_result(to_micro_seconds(MilliSeconds(500)));
        set_rf(Constant::SETRFBAUD, baud);
        check_result(to_micro_seconds(MilliSeconds(500)));

        boost::system::error_code ec;
        switch (baud)
        {
        case Constant::UP_9600BPS:
            port->set_option(boost::asio::serial_port::baud_rate(9600), ec);
            break;
        case Constant::UP_19200BPS:
            port->set_option(boost::asio::serial_port::baud_rate(19200), ec);
            break;
        case Constant::UP_38400BPS:
            port->set_option(boost::asio::serial_port::baud_rate(38400), ec);
            break;
        case Constant::UP_57600BPS:
            port->set_option(boost::asio::serial_port::baud_rate(57600), ec);
            break;
        case Constant::UP_115200BPS:
            port->set_option(boost::asio::serial_port::baud_rate(115200), ec);
            break;
        default:
            _shared->lastError = Constant::ILLIGICAL_CMD;
            return false;
            break;
        }
        if (ec)
        {
            _shared->lastError = Constant::DISCONNECT_COMM;
            return false;
        }
        set_rf(Constant::SETRFBAUD, baud);
        if (!commbox_do(Constant::SET_UPBAUD, &baud, 1))
            return false;
        if (!check_result(to_micro_seconds(MilliSeconds(500))))
            return false;
        stream().vs()->discard_in_buffer();
        return true;
    }

    bool set_pc_baud(boost::uint8_t baud)
    {
        std::size_t times = Constant::REPLAYTIMES;
        while (times--)
        {
            if (times == 0)
                return false;
            if (do_set_pc_baud(baud))
                break;
        }
        return true;
    }

    bool open_box()
    {
        std::size_t i;

        for (i = 0; i < 3; i++)
        {
            set_rf(Constant::RESET_RF, 0);
            set_rf(Constant::SETDTR_L, 0);
            if (init_box() && check_box())
            {
                stream().vs()->discard_in_buffer();
                if (stream().type() == BoxStream::SerialPort)
                {
                    return set_pc_baud(Constant::UP_57600BPS);
                }
                return true;
            }
        }

        return false;
    }

private:
    boost::uint32_t _buffUsedNum;
    boost::uint32_t _timeUnit;
    boost::uint32_t _timeBaseDB;
    boost::uint32_t _timeExternDB;
    boost::uint32_t _cmdBuffLen;
    boost::uint8_t _buffID;
    boost::uint8_t _headPassword;
    boost::array<boost::uint8_t, Constant::MAXIM_BLOCK + 2 > _buffAdd;
    boost::array<boost::uint8_t, Constant::MAXIM_BLOCK> _buffUsed;
    boost::array<boost::uint8_t, Constant::VERSIONLEN> _version;
    boost::array<boost::uint8_t, Constant::COMMBOXIDLEN> _boxID;
    boost::array<boost::uint8_t, Constant::COMMBOXPORTNUM> _port;
    StaticBytes _cmdTemp; // Write Command to buffer
};
}
}
}
}

#endif /* __JM_DIAG_v1_C168_H__ */
