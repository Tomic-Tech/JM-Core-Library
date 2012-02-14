#ifndef __JM_V1_BOX_W80_D_H__
#define __JM_V1_BOX_W80_D_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

#define JM_V1_BOX_W80_BOXINFO_LEN 12
#define JM_V1_BOX_W80_MAXPORT_NUM 4
#define JM_V1_BOX_W80_MAXBUFF_NUM 4
#define JM_V1_BOX_W80_MAXBUFF_LEN 0xA8
#define JM_V1_BOX_W80_LINKBLOCK 0x40
// 批处理执行次数 
#define JM_V1_BOX_W80_RUN_ONCE 0x00
#define JM_V1_BOX_W80_RUN_MORE 0x01
// 通讯校验和方式 
#define JM_V1_BOX_W80_CHECK_SUM 0x01
#define JM_V1_BOX_W80_CHECK_REVSUM 0x02
#define JM_V1_BOX_W80_CHECK_CRC 0x03
//////////////////////////////////////////////////////////////////////////
// 通讯口 PORT
//////////////////////////////////////////////////////////////////////////
#define JM_V1_BOX_W80_DH 0x80 // 高电平输出,1为关闭,0为打开 
#define JM_V1_BOX_W80_DL2 0x40 // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线 
#define JM_V1_BOX_W80_DL1 0x20 // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制 
#define JM_V1_BOX_W80_DL0 0x10 // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制 
#define JM_V1_BOX_W80_PWMS 0x08 // PWM发送线 
#define JM_V1_BOX_W80_PWMR 0x04
#define JM_V1_BOX_W80_COMS 0x02 // 标准发送通讯线路 
#define JM_V1_BOX_W80_COMR 0x01
#define JM_V1_BOX_W80_SET_NULL 0x00 // 不选择任何设置 

//////////////////////////////////////////////////////////////////////////
// 通讯物理控制口
//////////////////////////////////////////////////////////////////////////
#define JM_V1_BOX_W80_PWC 0x80 // 通讯电平控制,1为5伏,0为12伏 
#define JM_V1_BOX_W80_REFC 0x40 // 通讯比较电平控制,1为通讯电平1/5,0为比较电平控制1/2 
#define JM_V1_BOX_W80_CK 0x20 // K线控制开关,1为双线通讯,0为单线通讯 
#define JM_V1_BOX_W80_SZFC 0x10 // 发送逻辑控制,1为负逻辑,0为正逻辑 
#define JM_V1_BOX_W80_RZFC 0x08 // 接受逻辑控制,1为负逻辑,0为正逻辑 
#define JM_V1_BOX_W80_DLC0 0x04 // DLC1接受控制,1为接受关闭,0为接受打开 
#define JM_V1_BOX_W80_DLC1 0x02 // DLC0接受控制,1为接受关闭,0为接受打开 
#define JM_V1_BOX_W80_SLC 0x01 // 线选地址锁存器控制线(待用) 
#define JM_V1_BOX_W80_CLOSEALL 0x08 // 关闭所有发送口线，和接受口线 

//////////////////////////////////////////////////////////////////////////
// 通讯控制字1设定
//////////////////////////////////////////////////////////////////////////
#define JM_V1_BOX_W80_RS_232 0x00
#define JM_V1_BOX_W80_EXRS_232 0x20
#define JM_V1_BOX_W80_SET_VPW 0x40
#define JM_V1_BOX_W80_SET_PWM 0x60
#define JM_V1_BOX_W80_BIT9_SPACE 0x00
#define JM_V1_BOX_W80_BIT9_MARK 0x01
#define JM_V1_BOX_W80_BIT9_EVEN 0x02
#define JM_V1_BOX_W80_BIT9_ODD 0x03
#define JM_V1_BOX_W80_SEL_SL 0x00
#define JM_V1_BOX_W80_SEL_DL0 0x08
#define JM_V1_BOX_W80_SEL_DL1 0x10
#define JM_V1_BOX_W80_SEL_DL2 0x18
#define JM_V1_BOX_W80_SET_DB20 0x04
#define JM_V1_BOX_W80_UN_DB20 0x00

//////////////////////////////////////////////////////////////////////////
// 通讯控制字3设定
//////////////////////////////////////////////////////////////////////////
#define JM_V1_BOX_W80_ONEBYONE 0x80
#define JM_V1_BOX_W80_INVERTBYTE 0x40
#define JM_V1_BOX_W80_ORIGNALBYTE 0x00

//////////////////////////////////////////////////////////////////////////
// 接受命令类型定义
//////////////////////////////////////////////////////////////////////////
#define JM_V1_BOX_W80_WR_DATA 0x00
#define JM_V1_BOX_W80_WR_LINK 0xFF
#define JM_V1_BOX_W80_STOP_REC 0x04
#define JM_V1_BOX_W80_STOP_EXECUTE 0x08
#define JM_V1_BOX_W80_SET_UPBAUD 0x0C
#define JM_V1_BOX_W80_UP_9600BPS 0x00
#define JM_V1_BOX_W80_UP_19200BPS 0x01
#define JM_V1_BOX_W80_UP_38400BPS 0x02
#define JM_V1_BOX_W80_UP_57600BPS 0x03
#define JM_V1_BOX_W80_UP_115200BPS 0x04
#define JM_V1_BOX_W80_RESET 0x10
#define JM_V1_BOX_W80_GET_CPU 0x14
#define JM_V1_BOX_W80_GET_TIME 0x18
#define JM_V1_BOX_W80_GET_SET 0x1C
#define JM_V1_BOX_W80_GET_LINK 0x20
#define JM_V1_BOX_W80_GET_BUF 0x24
#define JM_V1_BOX_W80_GET_CMD 0x28
#define JM_V1_BOX_W80_GET_PORT 0x2C
#define JM_V1_BOX_W80_GET_BOXID 0x30

#define JM_V1_BOX_W80_DO_BAT_C 0x34
#define JM_V1_BOX_W80_DO_BAT_CN 0x38
#define JM_V1_BOX_W80_DO_BAT_L 0x3C
#define JM_V1_BOX_W80_DO_BAT_LN 0x40

#define JM_V1_BOX_W80_SET55_BAUD 0x44
#define JM_V1_BOX_W80_SET_ONEBYONE 0x48
#define JM_V1_BOX_W80_SET_BAUD 0x4C
#define JM_V1_BOX_W80_RUN_LINK 0x50
#define JM_V1_BOX_W80_STOP_LINK 0x54
#define JM_V1_BOX_W80_CLEAR_LINK 0x58
#define JM_V1_BOX_W80_GET_PORT1 0x5C

#define JM_V1_BOX_W80_SEND_DATA 0x60
#define JM_V1_BOX_W80_SET_CTRL 0x64
#define JM_V1_BOX_W80_SET_PORT0 0x68
#define JM_V1_BOX_W80_SET_PORT1 0x6C
#define JM_V1_BOX_W80_SET_PORT2 0x70
#define JM_V1_BOX_W80_SET_PORT3 0x74
#define JM_V1_BOX_W80_DELAYSHORT 0x78
#define JM_V1_BOX_W80_DELAYTIME 0x7C
#define JM_V1_BOX_W80_DELAYDWORD 0x80

#define JM_V1_BOX_W80_SETBYTETIME 0x88
#define JM_V1_BOX_W80_SETVPWSTART 0x08 // 最终要将SETVPWSTART转换成SETBYTETIME 
#define JM_V1_BOX_W80_SETWAITTIME 0x8C
#define JM_V1_BOX_W80_SETLINKTIME 0x90
#define JM_V1_BOX_W80_SETRECBBOUT 0x94
#define JM_V1_BOX_W80_SETRECFROUT 0x98
#define JM_V1_BOX_W80_SETVPWRECS 0x14 // 最终要将SETVPWRECS转换成SETRECBBOUT 

#define JM_V1_BOX_W80_COPY_BYTE 0x9C
#define JM_V1_BOX_W80_UPDATE_BYTE 0xA0
#define JM_V1_BOX_W80_INC_BYTE 0xA4
#define JM_V1_BOX_W80_DEC_BYTE 0xA8
#define JM_V1_BOX_W80_ADD_BYTE 0xAC
#define JM_V1_BOX_W80_SUB_BYTE 0xB0
#define JM_V1_BOX_W80_INVERT_BYTE 0xB4

#define JM_V1_BOX_W80_INC_DATA JM_V1_BOX_W80_INC_BYTE
#define JM_V1_BOX_W80_UPDATE_1BYTE JM_V1_BOX_W80_UPDATE_BYTE

#define JM_V1_BOX_W80_REC_FR 0xE0
#define JM_V1_BOX_W80_REC_LEN_1 0xE1
#define JM_V1_BOX_W80_REC_LEN_2 0xE2
#define JM_V1_BOX_W80_REC_LEN_3 0xE3
#define JM_V1_BOX_W80_REC_LEN_4 0xE4
#define JM_V1_BOX_W80_REC_LEN_5 0xE5
#define JM_V1_BOX_W80_REC_LEN_6 0xE6
#define JM_V1_BOX_W80_REC_LEN_7 0xE7
#define JM_V1_BOX_W80_REC_LEN_8 0xE8
#define JM_V1_BOX_W80_REC_LEN_9 0xE9
#define JM_V1_BOX_W80_REC_LEN_10 0xEA
#define JM_V1_BOX_W80_REC_LEN_11 0xEB
#define JM_V1_BOX_W80_REC_LEN_12 0xEC
#define JM_V1_BOX_W80_REC_LEN_13 0xED
#define JM_V1_BOX_W80_REC_LEN_14 0xEE
#define JM_V1_BOX_W80_REC_LEN_15 0xEF
#define JM_V1_BOX_W80_RECEIVE 0xF0

#define JM_V1_BOX_W80_RECV_ERR 0xAA // 接收错误 
#define JM_V1_BOX_W80_RECV_OK 0x55 // 接收正确 
#define JM_V1_BOX_W80_BUSY 0xBB // 开始执行 
#define JM_V1_BOX_W80_READY 0xDD // 执行结束 
#define JM_V1_BOX_W80_ERROR 0xEE // 执行错误 

// RF多对一的设定接口,最多16个 
#define JM_V1_BOX_W80_RF_RESET 0xD0
#define JM_V1_BOX_W80_RF_SETDTR_L 0xD1
#define JM_V1_BOX_W80_RF_SETDTR_H 0xD2
#define JM_V1_BOX_W80_RF_SET_BAUD 0xD3
#define JM_V1_BOX_W80_RF_SET_ADDR 0xD8
#define JM_V1_BOX_W80_COMMBOXID_ERR 1
#define JM_V1_BOX_W80_DISCONNECT_COMM 2
#define JM_V1_BOX_W80_DISCONNECT_COMMBOX 3
#define JM_V1_BOX_W80_OTHER_ERROR 4

#define JM_V1_BOX_W80_SK0 0
#define JM_V1_BOX_W80_SK1 1
#define JM_V1_BOX_W80_SK2 2
#define JM_V1_BOX_W80_SK3 3
#define JM_V1_BOX_W80_SK4 4
#define JM_V1_BOX_W80_SK5 5
#define JM_V1_BOX_W80_SK6 6
#define JM_V1_BOX_W80_SK7 7
#define JM_V1_BOX_W80_SK_NO 0xFF
#define JM_V1_BOX_W80_RK0 0
#define JM_V1_BOX_W80_RK1 1
#define JM_V1_BOX_W80_RK2 2
#define JM_V1_BOX_W80_RK3 3
#define JM_V1_BOX_W80_RK4 4
#define JM_V1_BOX_W80_RK5 5
#define JM_V1_BOX_W80_RK6 6
#define JM_V1_BOX_W80_RK7 7
#define JM_V1_BOX_W80_RK_NO 0xFF

#endif