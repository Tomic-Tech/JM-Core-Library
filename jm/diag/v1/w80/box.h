#ifndef __jm_DIAG_v1_w80_BOX_H__
#define __jm_DIAG_v1_w80_BOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/diag/v1/box.h>
#include <jm/diag/v1/w80/constant.h>
#include <jm/utils.h>

namespace JM
{
namespace Diag
{
namespace V1
{
namespace w80
{
class Box : public V1::Box<w80::Constant>
{
public:
    Box(const boost::shared_ptr<V1::Shared> &shared)
        : V1::Box<w80::Constant>(shared)
        , _pos(0)
    {
    }

    std::size_t read_data(boost::uint8_t *buff, std::size_t count, boost::int64_t microseconds)
    {
        std::size_t avail = 0;
        std::size_t len = 0;
        boost::system::error_code ec;
        stream().vs()->set_read_timeout(MicroSeconds(microseconds));
        len = stream().vs()->read(MutableBuffer(buff, count), ec);
        if (len < count)
        {
            avail = stream().vs()->bytes_available();
            if (avail > 0)
            {
                if (avail <= (count - len))
                {
                    len += stream().vs()->read(MutableBuffer(buff + len, avail), ec);
                }
                else
                {
                    len += stream().vs()->read(MutableBuffer(buff + len, count - len), ec);
                }
            }
        }
        return len;
    }

    std::size_t read_bytes(boost::uint8_t *buff, std::size_t count)
    {
        return read_data(buff, count, to_micro_seconds(MilliSeconds(500)));
    }

    bool set_line_level(boost::uint8_t valueLow, boost::uint8_t valueHigh)
    {
        _ports[1] &= ~valueLow;
        _ports[1] |= valueHigh;

        return do_set(Constant::SET_PORT1, _ports.data() + 1, 1);
    }

    bool set_comm_ctrl(boost::uint8_t valueOpen, boost::uint8_t valueClose)
    {
        _ports[2] &= ~valueOpen;
        _ports[2] |= valueClose;
        return do_set(Constant::SET_PORT2, _ports.data() + 2, 1);
    }

    bool set_comm_line(boost::uint8_t sendLine, boost::uint8_t recvLine)
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
        return do_set(Constant::SET_PORT0, _ports.data(), 1);
    }

    bool turn_over_one_by_one()
    {
        return do_set(Constant::ONEBYONE, NULL, 0);
    }

    bool keep_link(bool isRunLink)
    {
        if (isRunLink)
        {
            return do_set(Constant::RUN_LINK, NULL, 0);
        }
        else
        {
            return do_set(Constant::STOP_LINK, NULL, 0);
        }
    }

    bool set_comm_link(boost::uint8_t ctrlWord1, boost::uint8_t ctrlWord2, boost::uint8_t ctrlWord3)
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
            return do_set(Constant::SET_CTRL, ctrlWord.data(), 1);
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

        return do_set(Constant::SET_CTRL, ctrlWord.data(), length);
    }

    bool set_comm_baud(boost::uint32_t baud)
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

        baudTime[0] = high_byte(instructNum);
        baudTime[1] = low_byte(instructNum);

        if (baudTime[0] == 0)
        {
            return do_set(Constant::SET_BAUD, baudTime.data() + 1, 1);
        }
        return do_set(Constant::SET_BAUD, baudTime.data(), 2);
    }

    bool set_comm_time(boost::uint8_t type, boost::int64_t time)
    {
        boost::array<boost::uint8_t, 2> timeBuff;

        get_link_time(type, time);

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

        timeBuff[0] = high_byte(time);
        timeBuff[1] = low_byte(time);
        if (timeBuff[0] == 0)
        {
            return do_set(type, timeBuff.data() + 1, 1);
        }
        return do_set(type, timeBuff.data(), 2);
    }

    bool commbox_delay(boost::int64_t time)
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
        timeBuff[0] = high_byte(time);
        timeBuff[1] = low_byte(time);

        if (timeBuff[0] == 0)
        {
            return do_set(delayWord, timeBuff.data() + 1, 1);
        }
        return do_set(delayWord, timeBuff.data(), 2);
    }

    inline bool send_out_data(const boost::uint8_t *buffer, std::size_t count)
    {
        return do_set(Constant::SEND_DATA, buffer, count);
    }

    bool run_receive(boost::uint8_t type)
    {
        if (type == Constant::GET_PORT1)
        {
            _shared->isDB20 = false;
        }
        return do_cmd(type, NULL, 0);
    }

    bool stop_now(bool isStopExecute)
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
            if (stream().vs()->write(ConstBuffer(&cmd, 1), ec) != 1)
                continue;

            if (check_send())
            {
                if (isStopExecute &&
                        !check_result(to_micro_seconds(MilliSeconds(200))))
                    continue;
                return true;
            }
        }
        return false;
    }

    bool open_comm()
    {
        if (stream().type() == BoxStream::SerialPort)
        {
            JM::Asio::serial_port_ptr port = stream().serial_port();

            StringVector vec = port->get_system_ports();

            boost::system::error_code ec;
            for (StringVector::iterator it = vec.begin(); it != vec.end(); ++it)
            {
                port->set_option(JM::Asio::serial_port::baud_rate(115200), ec);
                port->set_option(JM::Asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::two), ec);
                port->set_option(JM::Asio::serial_port::parity(), ec);
                port->set_option(JM::Asio::serial_port::flow_control(), ec);
                port->set_option(JM::Asio::serial_port::character_size(), ec);
                port->open(*it, ec);
                if (!ec)
                {
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
            if (!stream().serial_port()->is_open())
                return true;
        }

        reset();
        set_rf(Constant::RF_RESET, 0);

        if  (stream().type() == BoxStream::SerialPort)
        {
            stream().serial_port()->close();
        }
        return true;
    }

    boost::uint8_t get_abs_add(boost::uint8_t buffID, boost::uint8_t add)
    {
        return 0;
    }

    bool update_buff(boost::uint8_t type, boost::uint8_t *buffer)
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
        return do_set(type, buf.data(), len);
    }

    bool new_batch(boost::uint8_t buffID)
    {
        _pos = 0;
        _isLink = (buffID == Constant::LINKBLOCK ? true : false);
        _shared->isDoNow = false;
        return true;
    }

    bool end_batch()
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

        return do_cmd(Constant::WR_DATA, _buf.data(), _pos);
    }

    bool del_batch(boost::uint8_t buffID)
    {
        _shared->isDoNow = true;

        _pos = 0;
        return true;

    }

    bool run_batch(boost::uint8_t *buffID, std::size_t count, bool isExecuteMany)
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
        return do_cmd(cmd, NULL, 0);
    }

    bool check_result(boost::int64_t microseconds)
    {
        boost::uint8_t rb = 0;

        boost::system::error_code ec;
        stream().vs()->set_read_timeout(MicroSeconds(microseconds));

        if (stream().vs()->read(MutableBuffer(&rb, 1), ec) != 1)
        {
            return false;
        }

        if (rb == Constant::READY || rb == Constant::ERR)
        {
            stream().vs()->discard_in_buffer();
            return true;
        }

        return false;
    }
private:
    bool check_idle()
    {
        boost::uint8_t rb = Constant::READY;
        boost::int32_t avail;

        avail = stream().vs()->bytes_available();

        if (avail > 20)
        {
            stream().vs()->discard_in_buffer();
            return true;
        }
        boost::system::error_code ec;

        while (stream().vs()->bytes_available())
        {
            stream().vs()->read(MutableBuffer(&rb, 1), ec);
        }

        if (rb == Constant::READY || rb == Constant::ERR)
        {
            return true;
        }

        if (stream().vs()->read(MutableBuffer(&rb, 1), ec) != 1)
        {
            return false;
        }

        if (rb == Constant::READY || rb == Constant::ERR)
        {
            return true;
        }

        return false;
    }

    bool check_send()
    {
        boost::system::error_code ec;
        boost::uint8_t rb = 0;
        if (stream().vs()->read(MutableBuffer(&rb, 1), ec) != 1)
        {
            return false;
        }

        if (rb == Constant::RECV_OK)
        {
            return true;
        }

        return false;
    }

    bool send_cmd(boost::uint8_t cmd, const boost::uint8_t *data, std::size_t count)
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
            if (!check_idle())
            {
                continue;
            }

            if (stream().vs()->write(ConstBuffer(command.get(), count + 2), ec) !=
                    count + 2)
            {
                continue;
            }

            if (check_send())
            {
                return true;
            }
        }
        return false;
    }

    bool do_cmd(boost::uint8_t cmd, const boost::uint8_t *buff, std::size_t count)
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
                ret = send_cmd(Constant::WR_DATA, temp.get(), count + 2);
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
                ret = send_cmd(Constant::WR_DATA, temp.get(), count + 4);
                if (!ret)
                {
                    return false;
                }
                return send_cmd(Constant::DO_BAT_C, NULL, 0);
            }
            else
            {
                return send_cmd(cmd, buff, count);
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

    bool do_set(boost::uint8_t cmd, const boost::uint8_t *buff, std::size_t count)
    {
        bool result = false;

        result = do_cmd(cmd, buff, count);
        if (result && _shared->isDoNow)
        {
            result = check_result(to_micro_seconds(MilliSeconds(150)));
        }
        return result;
    }

    boost::uint8_t get_buff_data(boost::uint8_t addr, boost::uint8_t *buff, std::size_t count)
    {
        boost::array<boost::uint8_t, 2> temp;
        temp[0] = addr;
        temp[1] = count;

        if (!do_cmd(Constant::GET_BUF, temp.data(), 2))
        {
            return 0;
        }

        return get_cmd_data(buff, count);
    }

    bool init_box()
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
            buf[i] = low_byte(rand());
        }

        for (i = 0; i < 10; ++i)
        {
            run += password[i] ^ buf[i % 3 + 1];
        }

        if (run == 0)
        {
            run = 0x55;
        }

        if (!do_cmd(Constant::GET_CPU, buf.data() + 1, 3))
        {
            return false;
        }
        if (get_cmd_data(buf.data(), 32) <= 0)
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

    boost::uint8_t get_cmd_data(boost::uint8_t *receiveBuffer, std::size_t count)
    {
        boost::uint8_t len = 0;
        boost::uint8_t cs = 0;

        if ((read_bytes(receiveBuffer, 1) != 1) ||
                (read_bytes(&len, 1) != 1))
        {
            return 0;
        }

        if (len > count)
        {
            len = count;
        }

        if (read_bytes(receiveBuffer, len) != len)
        {
            return 0;
        }

        if (read_bytes(&cs, 1) != 1)
        {
            return 0;
        }

        return len;
    }

    bool check_box()
    {
        boost::array<boost::uint8_t, 32> buf;

        if (!do_cmd(Constant::GET_BOXID, NULL, 0))
        {
            return false;
        }

        if (get_cmd_data(buf.data(), buf.size()) <= 0)
        {
            return false;
        }

        _boxVer = (buf[10] << 8) | buf[11];
        return true;
    }

    bool set_rf(boost::uint8_t cmd, boost::uint8_t data)
    {
        return false;
    }

    bool open_box()
    {
        if (init_box() && check_box())
        {
            stream().vs()->discard_in_buffer();
            stream().vs()->discard_out_buffer();
            return true;
        }
        return false;
    }

    bool reset()
    {
        std::size_t i;

        stop_now(true);
        stream().vs()->discard_out_buffer();
        stream().vs()->discard_in_buffer();

        for (i = 0; i < Constant::MAXPORT_NUM; ++i)
        {
            _ports[i] = 0xFF;
        }

        return do_cmd(Constant::RESET, NULL, 0);
    }

    bool copy_buff(boost::uint8_t dest, boost::uint8_t src, boost::uint8_t len)
    {
        boost::array<boost::uint8_t, 3> buf;

        buf[0] = dest;
        buf[1] = src;
        buf[2] = len;

        return do_set(Constant::COPY_BYTE, buf.data(), buf.size());
    }

    boost::uint32_t get_box_ver()
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