#ifndef __JM_V1_BOX_W80_C_HPP__
#define __JM_V1_BOX_W80_C_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

namespace JM
{
    namespace V1
    {
        namespace W80
        {
            class CONSTANT
            {
            public:
                static const guint8 BOXINFO_LEN = 12;
                static const guint8 MAXPORT_NUM = 4;
                static const guint8 MAXBUFF_NUM = 4;
                static const guint8 MAXBUFF_LEN = 0xA8;
                static const guint8 LINKBLOCK = 0x40;
                // 批处理执行次数 
                static const guint8 RUN_ONCE = 0x00;
                static const guint8 RUN_MORE = 0x01;
                // 通讯校验和方式 
                static const guint8 CHECK_SUM = 0x01;
                static const guint8 CHECK_REVSUM = 0x02;
                static const guint8 CHECK_CRC = 0x03;
                //////////////////////////////////////////////////////////////////////////
                // 通讯口 PORT
                //////////////////////////////////////////////////////////////////////////
                static const guint8 DH = 0x80; // 高电平输出,1为关闭,0为打开 
                static const guint8 DL2 = 0x40; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线 
                static const guint8 DL1 = 0x20; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制 
                static const guint8 DL0 = 0x10; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制 
                static const guint8 PWMS = 0x08; // PWM发送线 
                static const guint8 PWMR = 0x04;
                static const guint8 COMS = 0x02; // 标准发送通讯线路 
                static const guint8 COMR = 0x01;
                static const guint8 SET_NULL = 0x00; // 不选择任何设置 

                //////////////////////////////////////////////////////////////////////////
                // 通讯物理控制口
                //////////////////////////////////////////////////////////////////////////
                static const guint8 PWC = 0x80; // 通讯电平控制,1为5伏,0为12伏 
                static const guint8 REFC = 0x40; // 通讯比较电平控制,1为通讯电平1/5,0为比较电平控制1/2 
                static const guint8 CK = 0x20; // K线控制开关,1为双线通讯,0为单线通讯 
                static const guint8 SZFC = 0x10; // 发送逻辑控制,1为负逻辑,0为正逻辑 
                static const guint8 RZFC = 0x08; // 接受逻辑控制,1为负逻辑,0为正逻辑 
                static const guint8 DLC0 = 0x04; // DLC1接受控制,1为接受关闭,0为接受打开 
                static const guint8 DLC1 = 0x02; // DLC0接受控制,1为接受关闭,0为接受打开 
                static const guint8 SLC = 0x01; // 线选地址锁存器控制线(待用) 
                static const guint8 CLOSEALL = 0x08; // 关闭所有发送口线，和接受口线 

                //////////////////////////////////////////////////////////////////////////
                // 通讯控制字1设定
                //////////////////////////////////////////////////////////////////////////
                static const guint8 RS_232 = 0x00;
                static const guint8 EXRS_232 = 0x20;
                static const guint8 SET_VPW = 0x40;
                static const guint8 SET_PWM = 0x60;
                static const guint8 BIT9_SPACE = 0x00;
                static const guint8 BIT9_MARK = 0x01;
                static const guint8 BIT9_EVEN = 0x02;
                static const guint8 BIT9_ODD = 0x03;
                static const guint8 SEL_SL = 0x00;
                static const guint8 SEL_DL0 = 0x08;
                static const guint8 SEL_DL1 = 0x10;
                static const guint8 SEL_DL2 = 0x18;
                static const guint8 SET_DB20 = 0x04;
                static const guint8 UN_DB20 = 0x00;

                //////////////////////////////////////////////////////////////////////////
                // 通讯控制字3设定
                //////////////////////////////////////////////////////////////////////////
                static const guint8 ONEBYONE = 0x80;
                static const guint8 INVERTBYTE = 0x40;
                static const guint8 ORIGNALBYTE = 0x00;

                //////////////////////////////////////////////////////////////////////////
                // 接受命令类型定义
                //////////////////////////////////////////////////////////////////////////
                static const guint8 WR_DATA = 0x00;
                static const guint8 WR_LINK = 0xFF;
                static const guint8 STOP_REC = 0x04;
                static const guint8 STOP_EXECUTE = 0x08;
                static const guint8 SET_UPBAUD = 0x0C;
                static const guint8 UP_9600BPS = 0x00;
                static const guint8 UP_19200BPS = 0x01;
                static const guint8 UP_38400BPS = 0x02;
                static const guint8 UP_57600BPS = 0x03;
                static const guint8 UP_115200BPS = 0x04;
                static const guint8 RESET = 0x10;
                static const guint8 GET_CPU = 0x14;
                static const guint8 GET_TIME = 0x18;
                static const guint8 GET_SET = 0x1C;
                static const guint8 GET_LINK = 0x20;
                static const guint8 GET_BUF = 0x24;
                static const guint8 GET_CMD = 0x28;
                static const guint8 GET_PORT = 0x2C;
                static const guint8 GET_BOXID = 0x30;

                static const guint8 DO_BAT_C = 0x34;
                static const guint8 DO_BAT_CN = 0x38;
                static const guint8 DO_BAT_L = 0x3C;
                static const guint8 DO_BAT_LN = 0x40;

                static const guint8 SET55_BAUD = 0x44;
                static const guint8 SET_ONEBYONE = 0x48;
                static const guint8 SET_BAUD = 0x4C;
                static const guint8 RUN_LINK = 0x50;
                static const guint8 STOP_LINK = 0x54;
                static const guint8 CLEAR_LINK = 0x58;
                static const guint8 GET_PORT1 = 0x5C;

                static const guint8 SEND_DATA = 0x60;
                static const guint8 SET_CTRL = 0x64;
                static const guint8 SET_PORT0 = 0x68;
                static const guint8 SET_PORT1 = 0x6C;
                static const guint8 SET_PORT2 = 0x70;
                static const guint8 SET_PORT3 = 0x74;
                static const guint8 DELAYSHORT = 0x78;
                static const guint8 DELAYTIME = 0x7C;
                static const guint8 DELAYDWORD = 0x80;

                static const guint8 SETBYTETIME = 0x88;
                static const guint8 SETVPWSTART = 0x08; // 最终要将SETVPWSTART转换成SETBYTETIME 
                static const guint8 SETWAITTIME = 0x8C;
                static const guint8 SETLINKTIME = 0x90;
                static const guint8 SETRECBBOUT = 0x94;
                static const guint8 SETRECFROUT = 0x98;
                static const guint8 SETVPWRECS = 0x14; // 最终要将SETVPWRECS转换成SETRECBBOUT 

                static const guint8 COPY_BYTE = 0x9C;
                static const guint8 UPDATE_BYTE = 0xA0;
                static const guint8 INC_BYTE = 0xA4;
                static const guint8 DEC_BYTE = 0xA8;
                static const guint8 ADD_BYTE = 0xAC;
                static const guint8 SUB_BYTE = 0xB0;
                static const guint8 INVERT_BYTE = 0xB4;

                static const guint8 INC_DATA = INC_BYTE;
                static const guint8 UPDATE_1BYTE = UPDATE_BYTE;

                static const guint8 REC_FR = 0xE0;
                static const guint8 REC_LEN_1 = 0xE1;
                static const guint8 REC_LEN_2 = 0xE2;
                static const guint8 REC_LEN_3 = 0xE3;
                static const guint8 REC_LEN_4 = 0xE4;
                static const guint8 REC_LEN_5 = 0xE5;
                static const guint8 REC_LEN_6 = 0xE6;
                static const guint8 REC_LEN_7 = 0xE7;
                static const guint8 REC_LEN_8 = 0xE8;
                static const guint8 REC_LEN_9 = 0xE9;
                static const guint8 REC_LEN_10 = 0xEA;
                static const guint8 REC_LEN_11 = 0xEB;
                static const guint8 REC_LEN_12 = 0xEC;
                static const guint8 REC_LEN_13 = 0xED;
                static const guint8 REC_LEN_14 = 0xEE;
                static const guint8 REC_LEN_15 = 0xEF;
                static const guint8 RECEIVE = 0xF0;

                static const guint8 RECV_ERR = 0xAA; // 接收错误 
                static const guint8 RECV_OK = 0x55; // 接收正确 
                static const guint8 BUSY = 0xBB; // 开始执行 
                static const guint8 READY = 0xDD; // 执行结束 
                static const guint8 ERR = 0xEE; // 执行错误 

                // RF多对一的设定接口,最多16个 
                static const guint8 RF_RESET = 0xD0;
                static const guint8 RF_SETDTR_L = 0xD1;
                static const guint8 RF_SETDTR_H = 0xD2;
                static const guint8 RF_SET_BAUD = 0xD3;
                static const guint8 RF_SET_ADDR = 0xD8;
                static const guint8 COMMBOXID_ERR = 1;
                static const guint8 DISCONNECT_COMM = 2;
                static const guint8 DISCONNECT_COMMBOX = 3;
                static const guint8 OTHER_ERROR = 4;

                static const guint8 SK0 = 0;
                static const guint8 SK1 = 1;
                static const guint8 SK2 = 2;
                static const guint8 SK3 = 3;
                static const guint8 SK4 = 4;
                static const guint8 SK5 = 5;
                static const guint8 SK6 = 6;
                static const guint8 SK7 = 7;
                static const guint8 SK_NO = 0xFF;
                static const guint8 RK0 = 0;
                static const guint8 RK1 = 1;
                static const guint8 RK2 = 2;
                static const guint8 RK3 = 3;
                static const guint8 RK4 = 4;
                static const guint8 RK5 = 5;
                static const guint8 RK6 = 6;
                static const guint8 RK7 = 7;
                static const guint8 RK_NO = 0xFF;
            };
        }
    }
}


#endif