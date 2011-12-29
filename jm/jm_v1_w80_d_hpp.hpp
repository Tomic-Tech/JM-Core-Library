/* 
 * File:   jm_v1_w80_d_hpp.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月29日, 下午6:10
 */

#ifndef JM_V1_W80_D_HPP_HPP
#define	JM_V1_W80_D_HPP_HPP

#include <jm_types.hpp>

namespace jm {
namespace v1 {

struct v1_w80_d {
    static const uint32 BOXINFO_LEN = 12;
    static const uint32 MAXPORT_NUM = 4;
    static const uint32 MAXBUFF_NUM = 4;
    static const uint32 MAXBUFF_LEN = 0xA8;
    static const uint32 LINKBLOCK = 0x40;
    //批处理执行次数
    static const uint32 RUN_ONCE = 0x00;
    static const uint32 RUN_MORE = 0x01;
    //通讯校验和方式
    static const uint32 CHECK_SUM = 0x01;
    static const uint32 CHECK_REVSUM = 0x02;
    static const uint32 CHECK_CRC = 0x03;
    ///////////////////////////////////////////////////////////////////////////////
    //  通讯口 PORT
    ///////////////////////////////////////////////////////////////////////////////
    static const uint8 DH = 0x80; //高电平输出,1为关闭,0为打开
    static const uint8 DL2 = 0x40; //低电平输出,1为关闭,0为打开,正逻辑发送通讯线
    static const uint8 DL1 = 0x20; //低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制
    static const uint8 DL0 = 0x10; //低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制
    static const uint8 PWMS = 0x08; //PWM发送线
    static const uint8 PWMR = 0x04;
    static const uint8 COMS = 0x02; //标准发送通讯线路
    static const uint8 COMR = 0x01;
    static const uint8 SET_NULL = 0x00; //不选择任何设置

    ///////////////////////////////////////////////////////////////////////////////
    //  通讯物理控制口
    ///////////////////////////////////////////////////////////////////////////////
    static const uint8 PWC = 0x80; //通讯电平控制,1为5伏,0为12伏
    static const uint8 REFC = 0x40; //通讯比较电平控制,1为通讯电平1/5,0为比较电平控制1/2
    static const uint8 CK = 0x20; //K线控制开关,1为双线通讯,0为单线通讯
    static const uint8 SZFC = 0x10; //发送逻辑控制,1为负逻辑,0为正逻辑
    static const uint8 RZFC = 0x08; //接受逻辑控制,1为负逻辑,0为正逻辑
    static const uint8 DLC0 = 0x04; //DLC1接受控制,1为接受关闭,0为接受打开
    static const uint8 DLC1 = 0x02; //DLC0接受控制,1为接受关闭,0为接受打开
    static const uint8 SLC = 0x01; //线选地址锁存器控制线(待用)
    static const uint8 CLOSEALL = 0x08; //关闭所有发送口线，和接受口线

    ///////////////////////////////////////////////////////////////////////////////
    //  通讯控制字1设定
    ///////////////////////////////////////////////////////////////////////////////
    static const uint8 RS_232 = 0x00;
    static const uint8 EXRS_232 = 0x20;
    static const uint8 SET_VPW = 0x40;
    static const uint8 SET_PWM = 0x60;
    static const uint8 BIT9_SPACE = 0x00;
    static const uint8 BIT9_MARK = 0x01;
    static const uint8 BIT9_EVEN = 0x02;
    static const uint8 BIT9_ODD = 0x03;
    static const uint8 SEL_SL = 0x00;
    static const uint8 SEL_DL0 = 0x08;
    static const uint8 SEL_DL1 = 0x10;
    static const uint8 SEL_DL2 = 0x18;
    static const uint8 SET_DB20 = 0x04;
    static const uint8 UN_DB20 = 0x00;

    ///////////////////////////////////////////////////////////////////////////////
    //  通讯控制字3设定
    ///////////////////////////////////////////////////////////////////////////////
    static const uint8 ONEBYONE = 0x80;
    static const uint8 INVERTBYTE = 0x40;
    static const uint8 ORIGNALBYTE = 0x00;
    ///////////////////////////////////////////////////////////////////////////////
    //  接受命令类型定义
    ///////////////////////////////////////////////////////////////////////////////
    static const uint8 WR_DATA = 0x00;
    static const uint8 WR_LINK = 0xFF;
    static const uint8 STOP_REC = 0x04;
    static const uint8 STOP_EXECUTE = 0x08;
    static const uint8 SET_UPBAUD = 0x0c;
    static const uint8 UP_9600BPS = 0x00;
    static const uint8 UP_19200BPS = 0x01;
    static const uint8 UP_38400BPS = 0x02;
    static const uint8 UP_57600BPS = 0x03;
    static const uint8 UP_115200BPS = 0x04;
    static const uint8 RESET = 0x10;
    static const uint8 GET_CPU = 0x14;
    static const uint8 GET_TIME = 0x18;
    static const uint8 GET_SET = 0x1c;
    static const uint8 GET_LINK = 0x20;
    static const uint8 GET_BUF = 0x24;
    static const uint8 GET_CMD = 0x28;
    static const uint8 GET_PORT = 0x2c;
    static const uint8 GET_BOXID = 0x30;

    static const uint8 DO_BAT_C = 0x34;
    static const uint8 DO_BAT_CN = 0x38;
    static const uint8 DO_BAT_L = 0x3c;
    static const uint8 DO_BAT_LN = 0x40;

    static const uint8 SET55_BAUD = 0x44;
    static const uint8 SET_ONEBYONE = 0x48;
    static const uint8 SET_BAUD = 0x4c;
    static const uint8 RUN_LINK = 0x50;
    static const uint8 STOP_LINK = 0x54;
    static const uint8 CLEAR_LINK = 0x58;
    static const uint8 GET_PORT1 = 0x5C;

    static const uint8 SEND_DATA = 0x60;
    static const uint8 SET_CTRL = 0x64;
    static const uint8 SET_PORT0 = 0x68;
    static const uint8 SET_PORT1 = 0x6c;
    static const uint8 SET_PORT2 = 0x70;
    static const uint8 SET_PORT3 = 0x74;
    static const uint8 DELAYSHORT = 0x78;
    static const uint8 DELAYTIME = 0x7c;
    static const uint8 DELAYDWORD = 0x80;

    static const uint8 SETBYTETIME = 0x88;
    static const uint8 SETVPWSTART = 0x08; //最终要将SETVPWSTART转换成SETBYTETIME
    static const uint8 SETWAITTIME = 0x8c;
    static const uint8 SETLINKTIME = 0x90;
    static const uint8 SETRECBBOUT = 0x94;
    static const uint8 SETRECFROUT = 0x98;
    static const uint8 SETVPWRECS = 0x14; //最终要将SETVPWRECS转换成SETRECBBOUT

    static const uint8 COPY_BYTE = 0x9C;
    static const uint8 UPDATE_BYTE = 0xA0;
    static const uint8 INC_BYTE = 0xA4;
    static const uint8 DEC_BYTE = 0xA8;
    static const uint8 ADD_BYTE = 0xAC;
    static const uint8 SUB_BYTE = 0xB0;
    static const uint8 INVERT_BYTE = 0xB4;

    static const uint8 REC_FR = 0xE0;
    static const uint8 REC_LEN_1 = 0xE1;
    static const uint8 REC_LEN_2 = 0xE2;
    static const uint8 REC_LEN_3 = 0xE3;
    static const uint8 REC_LEN_4 = 0xE4;
    static const uint8 REC_LEN_5 = 0xE5;
    static const uint8 REC_LEN_6 = 0xE6;
    static const uint8 REC_LEN_7 = 0xE7;
    static const uint8 REC_LEN_8 = 0xE8;
    static const uint8 REC_LEN_9 = 0xE9;
    static const uint8 REC_LEN_10 = 0xEA;
    static const uint8 REC_LEN_11 = 0xEB;
    static const uint8 REC_LEN_12 = 0xEC;
    static const uint8 REC_LEN_13 = 0xED;
    static const uint8 REC_LEN_14 = 0xEE;
    static const uint8 REC_LEN_15 = 0xEF;
    static const uint8 RECEIVE = 0xF0;

    static const uint8 RECV_ERR = 0xAA; //接收错误   			
    static const uint8 RECV_OK = 0x55; //接收正确
    static const uint8 BUSY = 0xBB; //开始执行	
    static const uint8 READY = 0xDD; //执行结束
    static const uint8 Error = 0xEE; //执行错误	

    //RF多对一的设定接口,最多16个
    static const uint8 RF_RESET = 0xD0;
    static const uint8 RF_SETDTR_L = 0xD1;
    static const uint8 RF_SETDTR_H = 0xD2;
    static const uint8 RF_SET_BAUD = 0xD3;
    static const uint8 RF_SET_ADDR = 0xD8;
    static const uint8 COMMBOXID_ERR = 1;
    static const uint8 DISCONNECT_COMM = 2;
    static const uint8 DISCONNECT_COMBOX = 3;
    static const uint8 OTHER_ERROR = 4;

};

}
}

#endif	/* JM_V1_W80_D_HPP_HPP */

