#ifndef __JM_DIAG_V1_W80_CONSTANT_HPP__
#define __JM_DIAG_V1_W80_CONSTANT_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/cstdint.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
namespace W80
{
struct Constant
{
    static const boost::uint8_t BOXINFO_LEN = 12;
    static const boost::uint8_t MAXPORT_NUM = 4;
    static const boost::uint8_t MAXBUFF_NUM = 4;
    static const boost::uint8_t MAXBUFF_LEN = 0xA8;
    static const boost::uint8_t LINKBLOCK = 0x40;
    // ������ִ�д���
    static const boost::uint8_t RUN_ONCE = 0x00;
    static const boost::uint8_t RUN_MORE = 0x01;
    // ͨѶУ��ͷ�ʽ
    static const boost::uint8_t CHECK_SUM = 0x01;
    static const boost::uint8_t CHECK_REVSUM = 0x02;
    static const boost::uint8_t CHECK_CRC = 0x03;
    //////////////////////////////////////////////////////////////////////////
    // ͨѶ�� PORT
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t DH = 0x80; // �ߵ�ƽ���,1Ϊ�ر�,0Ϊ��
    static const boost::uint8_t DL2 = 0x40; // �͵�ƽ���,1Ϊ�ر�,0Ϊ��,���߼�����ͨѶ��
    static const boost::uint8_t DL1 = 0x20; // �͵�ƽ���,1Ϊ�ر�,0Ϊ��,���߼�����ͨѶ��,�����ܿ���
    static const boost::uint8_t DL0 = 0x10; // �͵�ƽ���,1Ϊ�ر�,0Ϊ��,���߼�����ͨѶ��,�����ܿ���
    static const boost::uint8_t PWMS = 0x08; // PWM������
    static const boost::uint8_t PWMR = 0x04;
    static const boost::uint8_t COMS = 0x02; // ��׼����ͨѶ��·
    static const boost::uint8_t COMR = 0x01;
    static const boost::uint8_t SET_NULL = 0x00; // ��ѡ���κ�����

    //////////////////////////////////////////////////////////////////////////
    // ͨѶ�������ƿ�
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t PWC = 0x80; // ͨѶ��ƽ����,1Ϊ5��,0Ϊ12��
    static const boost::uint8_t REFC = 0x40; // ͨѶ�Ƚϵ�ƽ����,1ΪͨѶ��ƽ1/5,0Ϊ�Ƚϵ�ƽ����1/2
    static const boost::uint8_t CK = 0x20; // K�߿��ƿ���,1Ϊ˫��ͨѶ,0Ϊ����ͨѶ
    static const boost::uint8_t SZFC = 0x10; // �����߼�����,1Ϊ���߼�,0Ϊ���߼�
    static const boost::uint8_t RZFC = 0x08; // �����߼�����,1Ϊ���߼�,0Ϊ���߼�
    static const boost::uint8_t DLC0 = 0x04; // DLC1���ܿ���,1Ϊ���ܹر�,0Ϊ���ܴ�
    static const boost::uint8_t DLC1 = 0x02; // DLC0���ܿ���,1Ϊ���ܹر�,0Ϊ���ܴ�
    static const boost::uint8_t SLC = 0x01; // ��ѡ��ַ������������(����)
    static const boost::uint8_t CLOSEALL = 0x08; // �ر����з��Ϳ��ߣ��ͽ��ܿ���

    //////////////////////////////////////////////////////////////////////////
    // ͨѶ������1�趨
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t RS_232 = 0x00;
    static const boost::uint8_t EXRS_232 = 0x20;
    static const boost::uint8_t SET_VPW = 0x40;
    static const boost::uint8_t SET_PWM = 0x60;
    static const boost::uint8_t BIT9_SPACE = 0x00;
    static const boost::uint8_t BIT9_MARK = 0x01;
    static const boost::uint8_t BIT9_EVEN = 0x02;
    static const boost::uint8_t BIT9_ODD = 0x03;
    static const boost::uint8_t SEL_SL = 0x00;
    static const boost::uint8_t SEL_DL0 = 0x08;
    static const boost::uint8_t SEL_DL1 = 0x10;
    static const boost::uint8_t SEL_DL2 = 0x18;
    static const boost::uint8_t SET_DB20 = 0x04;
    static const boost::uint8_t UN_DB20 = 0x00;

    //////////////////////////////////////////////////////////////////////////
    // ͨѶ������3�趨
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t ONEBYONE = 0x80;
    static const boost::uint8_t INVERTBYTE = 0x40;
    static const boost::uint8_t ORIGNALBYTE = 0x00;

    //////////////////////////////////////////////////////////////////////////
    // �����������Ͷ���
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t WR_DATA = 0x00;
    static const boost::uint8_t WR_LINK = 0xFF;
    static const boost::uint8_t STOP_REC = 0x04;
    static const boost::uint8_t STOP_EXECUTE = 0x08;
    static const boost::uint8_t SET_UPBAUD = 0x0C;
    static const boost::uint8_t UP_9600BPS = 0x00;
    static const boost::uint8_t UP_19200BPS = 0x01;
    static const boost::uint8_t UP_38400BPS = 0x02;
    static const boost::uint8_t UP_57600BPS = 0x03;
    static const boost::uint8_t UP_115200BPS = 0x04;
    static const boost::uint8_t RESET = 0x10;
    static const boost::uint8_t GET_CPU = 0x14;
    static const boost::uint8_t GET_TIME = 0x18;
    static const boost::uint8_t GET_SET = 0x1C;
    static const boost::uint8_t GET_LINK = 0x20;
    static const boost::uint8_t GET_BUF = 0x24;
    static const boost::uint8_t GET_CMD = 0x28;
    static const boost::uint8_t GET_PORT = 0x2C;
    static const boost::uint8_t GET_BOXID = 0x30;

    static const boost::uint8_t DO_BAT_C = 0x34;
    static const boost::uint8_t DO_BAT_CN = 0x38;
    static const boost::uint8_t DO_BAT_L = 0x3C;
    static const boost::uint8_t DO_BAT_LN = 0x40;

    static const boost::uint8_t SET55_BAUD = 0x44;
    static const boost::uint8_t SET_ONEBYONE = 0x48;
    static const boost::uint8_t SET_BAUD = 0x4C;
    static const boost::uint8_t RUN_LINK = 0x50;
    static const boost::uint8_t STOP_LINK = 0x54;
    static const boost::uint8_t CLEAR_LINK = 0x58;
    static const boost::uint8_t GET_PORT1 = 0x5C;

    static const boost::uint8_t SEND_DATA = 0x60;
    static const boost::uint8_t SET_CTRL = 0x64;
    static const boost::uint8_t SET_PORT0 = 0x68;
    static const boost::uint8_t SET_PORT1 = 0x6C;
    static const boost::uint8_t SET_PORT2 = 0x70;
    static const boost::uint8_t SET_PORT3 = 0x74;
    static const boost::uint8_t DELAYSHORT = 0x78;
    static const boost::uint8_t DELAYTIME = 0x7C;
    static const boost::uint8_t DELAYDWORD = 0x80;

    static const boost::uint8_t SETBYTETIME = 0x88;
    static const boost::uint8_t SETVPWSTART = 0x08; // ����Ҫ��SETVPWSTARTת����SETBYTETIME
    static const boost::uint8_t SETWAITTIME = 0x8C;
    static const boost::uint8_t SETLINKTIME = 0x90;
    static const boost::uint8_t SETRECBBOUT = 0x94;
    static const boost::uint8_t SETRECFROUT = 0x98;
    static const boost::uint8_t SETVPWRECS = 0x14; // ����Ҫ��SETVPWRECSת����SETRECBBOUT

    static const boost::uint8_t COPY_BYTE = 0x9C;
    static const boost::uint8_t UPDATE_BYTE = 0xA0;
    static const boost::uint8_t INC_BYTE = 0xA4;
    static const boost::uint8_t DEC_BYTE = 0xA8;
    static const boost::uint8_t ADD_BYTE = 0xAC;
    static const boost::uint8_t SUB_BYTE = 0xB0;
    static const boost::uint8_t INVERT_BYTE = 0xB4;

    static const boost::uint8_t INC_DATA = INC_BYTE;
    static const boost::uint8_t UPDATE_1BYTE = UPDATE_BYTE;

    static const boost::uint8_t REC_FR = 0xE0;
    static const boost::uint8_t REC_LEN_1 = 0xE1;
    static const boost::uint8_t REC_LEN_2 = 0xE2;
    static const boost::uint8_t REC_LEN_3 = 0xE3;
    static const boost::uint8_t REC_LEN_4 = 0xE4;
    static const boost::uint8_t REC_LEN_5 = 0xE5;
    static const boost::uint8_t REC_LEN_6 = 0xE6;
    static const boost::uint8_t REC_LEN_7 = 0xE7;
    static const boost::uint8_t REC_LEN_8 = 0xE8;
    static const boost::uint8_t REC_LEN_9 = 0xE9;
    static const boost::uint8_t REC_LEN_10 = 0xEA;
    static const boost::uint8_t REC_LEN_11 = 0xEB;
    static const boost::uint8_t REC_LEN_12 = 0xEC;
    static const boost::uint8_t REC_LEN_13 = 0xED;
    static const boost::uint8_t REC_LEN_14 = 0xEE;
    static const boost::uint8_t REC_LEN_15 = 0xEF;
    static const boost::uint8_t RECEIVE = 0xF0;

    static const boost::uint8_t RECV_ERR = 0xAA; // ���մ���
    static const boost::uint8_t RECV_OK = 0x55; // ������ȷ
    static const boost::uint8_t BUSY = 0xBB; // ��ʼִ��
    static const boost::uint8_t READY = 0xDD; // ִ�н���
    static const boost::uint8_t ERR = 0xEE; // ִ�д���

    // RF���һ���趨�ӿ�,���16��
    static const boost::uint8_t RF_RESET = 0xD0;
    static const boost::uint8_t RF_SETDTR_L = 0xD1;
    static const boost::uint8_t RF_SETDTR_H = 0xD2;
    static const boost::uint8_t RF_SET_BAUD = 0xD3;
    static const boost::uint8_t RF_SET_ADDR = 0xD8;
    static const boost::uint8_t COMMBOXID_ERR = 1;
    static const boost::uint8_t DISCONNECT_COMM = 2;
    static const boost::uint8_t DISCONNECT_COMMBOX = 3;
    static const boost::uint8_t OTHER_ERROR = 4;

    static const boost::uint8_t SK0 = 0;
    static const boost::uint8_t SK1 = 1;
    static const boost::uint8_t SK2 = 2;
    static const boost::uint8_t SK3 = 3;
    static const boost::uint8_t SK4 = 4;
    static const boost::uint8_t SK5 = 5;
    static const boost::uint8_t SK6 = 6;
    static const boost::uint8_t SK7 = 7;
    static const boost::uint8_t SK_NO = 0xFF;
    static const boost::uint8_t RK0 = 0;
    static const boost::uint8_t RK1 = 1;
    static const boost::uint8_t RK2 = 2;
    static const boost::uint8_t RK3 = 3;
    static const boost::uint8_t RK4 = 4;
    static const boost::uint8_t RK5 = 5;
    static const boost::uint8_t RK6 = 6;
    static const boost::uint8_t RK7 = 7;
    static const boost::uint8_t RK_NO = 0xFF;
};
}
}
}
}

#endif