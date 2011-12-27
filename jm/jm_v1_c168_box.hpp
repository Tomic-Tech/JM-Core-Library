/* 
 * File:   jm_v1_c168_box.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月23日, 下午5:31
 */

#ifndef JM_V1_C168_BOX_HPP
#define	JM_V1_C168_BOX_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_v1_box.hpp>
#include <jm_port.hpp>

namespace jm {

class v1_c168_box : public v1_box {
public:
    // /////////////////////////////////////////////////////
    // CommBox 固定信息 宏定义表
    // /////////////////////////////////////////////////////
    static const uint8 NULLADD = 0xFF; // 表示此块无使用
    static const uint32 MAXIM_BLOCK = 0x40; // 命令缓从区的最大数
    static const uint32 SWAPBLOCK = MAXIM_BLOCK + 1; // 数据交换区的块表识
    static const uint32 START_BAUD = 57600; // 上位机同下位机通信在复位或上电时波特率为57600
    static const uint32 CMD_DATALEN = 4; // 非发送命令最大长度
    // /////////////////////////////////////////////////////
    /*
     * // P1口为通讯口 #define DH 0x80 //高电平输出,1为关闭,0为打开 #define DL2 0x40
     * //低电平输出,1为关闭,0为打开,正逻辑发送通讯线 #define DL1 0x20
     * //低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制 #define DL0 0x10
     * //低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制 #define PWMS 0x08 //PWM发送线 #define COMS
     * 0x02 //标准发送通讯线路 #define SET_NULL 0x00 //不选择任何设置
     * 
     * //P2口为通讯物理控制口 #define PWC 0x80 //通讯电平控制,1为5伏,0为12伏 #define REFC 0x40
     * //通讯比较电平控制,1为通讯电平1/5,0为比较电平控制1/2 #define CK 0x20 //K线控制开关,1为双线通讯,0为单线通讯
     * #define SZFC 0x10 //发送逻辑控制,1为负逻辑,0为正逻辑 #define RZFC 0x08
     * //接受逻辑控制,1为负逻辑,0为正逻辑 #define DLC1 0x04 //DLC1接受控制,1为接受关闭,0为接受打开 #define
     * DLC0 0x02 //DLC0接受控制,1为接受关闭,0为接受打开 #define SLC 0x01 //线选地址锁存器控制线(待用) //
     * P0口选线控制 #define CLOSEALL 0x08
     * 
     * // 通讯控制字设定 #define RS_232 0x00 //通讯控制字1 #define EXRS_232 0x20 //通讯控制字1
     * #define SET_VPW 0x40 //通讯控制字1 #define SET_PWM 0x60 //通讯控制字1 #define
     * BIT9_SPACE 0x00 //通讯控制字1 #define BIT9_MARK 0x01 //通讯控制字1 #define
     * BIT9_EVEN 0x02 //通讯控制字1 #define BIT9_ODD 0x03 //通讯控制字1 #define SEL_SL
     * 0x00 //通讯控制字1 #define SEL_DL0 0x08 //通讯控制字1 #define SEL_DL1 0x10 //通讯控制字1
     * #define SEL_DL2 0x18 //通讯控制字1 #define SET_DB20 0x04 //通讯控制字1 #define
     * UN_DB20 0x00 //通讯控制字1 #define ONEBYONE 0x80 //通讯控制字3 #define INVERTBYTE
     * 0x40 //通讯控制字3 #define ORIGNALBYTE 0X00 //通讯控制字3
     */
    /***************************************************************************
     * 命令定义区: 命令分为四类: 1、写入命令缓冲区命令： 将以整理好的批处理命令写入缓冲区：格式如下 命令字 WR_DATA 0xD0?+
     * 长度（数据[N]+地址） +写入缓冲区地址+命令1+ 命令2。。。+命令N+校验。 其中命令N：为不含校验的命令，校验方法：为校验和
     * 命令区存放格式为：长度（数据[N]+地址） +写入缓冲区地址+命令1+ 命令2。。。+命令N 2、单字节命令：（大于写入命令缓冲区命令字
     * WR_DATA 0xD0，皆为单字节命令区） 简称快速命令：格式如下 命令字+校验和： 非缓冲区命令： 其中中断命令2个：停止执行，停止接受
     * 软件复位，得到命令缓冲区数据，得到链路保持数据，得到上次缓冲区命令的数据 缓冲区命令： 1、缓冲区数据操作命令。 2、开关命令 3、链路保持命令
     * 4、接受命令 3、多字节命令：（命令空间 0x30-0xCF） 格式如下：命令字（6BIT）+长度（数据长度-1；2BIT）+数据[N]+校验和
     * 1、设置命令 2、数据操作命令
     * 
     * 4、发送命令：（命令空间 0x00-0x2F） 格式如下： 长度（数据[N]+1）+数据[N]+校验和
     * 发送命令在写入缓从区时长度可以有0x2F，有0x30个数据，但不写入缓冲区直接发送，追多不超过4个 5、中断命令2个：停止执行，停止接受
     * 发送命令字，无校验，仅为一个字节，无运行返回，以等待运行结果标志返回。
     ***************************************************************************/
    // 1、写入命令缓冲区命令：
    static const uint8 WR_DATA = 0xD0; // 写缓冲区命令字,写入数据到命令缓冲区
    static const uint8 WR_LINK = 0xFF; // 若写入命令的地址为WR_LINK
    // ，写入数据到链路保持区链路保持区存放在命令缓冲区最后,存放次序:按地址从低到高
    static const uint8 SEND_LEN = 0xA8; // 一次发送数据的数据长度,0X70个数据
    static const uint8 SEND_DATA = 0x00; //0x60//0x00;
    // 2、单字节命令：（大于写入命令缓冲区命令字 WR_DATA = 0xD0，皆为单字节命令区）
    // 非缓冲区命令
    static const uint8 RESET = 0xF3; //0x10//0xF3; // 软件复位命令 清除所有缓冲区和寄存器内容。
    static const uint8 GETINFO = 0xF4;
    /*
     * 得到CPU速度 F9 返回CPU的指令执行时间（按纳秒计，数值传递，3个字节） 和时间控制参数 返回时间控制的指令执行数：
     * 其他控制（1byte）（DB20） 长等待控制（1byte）（DB200） 缓冲区长度（1byte） 产品序号（10byte） 和版本信息
     * 返回Commbox的硬件版本号。 等待接受5字节密码：（第五个字节为校验和）同公钥循环与或的校验和，返回命令增值。
     */
    /*
     * #define GET_TIME 0xF5 //得到时间设定 DD 返回字节时间、等待发送时间、链路保持时间、字节超时时间、接受超时时间
     * #define GET_SET 0xF6 //得到链路设定 DE 返回链路控制字(3字节)、通讯波特率 #define GET_PORT 0xF7
     * //得到端口设置 DF 返回端口p0，p1，p2，p3 #define GET_LINKDATA 0xF8 //得到链路数据 FC
     * 返回链路保持命令块中的所有内容 (中断命令) #define GET_BUFFDATA 0xF9 //得到缓冲器数据 FD 返回整个缓冲区数据
     * (中断命令) #define GET_CMMAND 0xFA //得到命令数据 FE 返回上一执行命令。 (中断命令)
     */
    // 中断命令定义
    static const uint8 STOP_REC = 0xFB; //0x04//0xFB; // 中断接受命令 强行退出当前接受命令，不返回错误。(中断命令)
    static const uint8 STOP_EXECUTE = 0xFC; //0x08//0xFC; // 中断批处理命令
    // 在当前执行时，通过该命令停止当前接受操作，返回错误。(中断命令)
    // 单字节缓冲区命令
    // #define GET_PORT1 0xD8 //等到通讯口的当前状态
    static const uint8 SET_ONEBYONE = 0xD9; //0x48//0xD9; // 将原有的接受一个发送一个的标志翻转

    /*
     * #define SET55_BAUD 0xDA //计算0x55的波特率 #define REC_FR 0xE0 //接受一帧命令 E0
     * 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传， //待中断当前命令和中断处理命令，当接受的字节超过字节间的最大时间，自动正常退出。
     * //若设定了长度接受,超时最长等待时间,自动返回. #define REC_LEN 0xE1 //接受长度数据 E1-EF
     * 开始时回传开始接受信号，接受命令字节低四位为长度的数据自动退出，
     * //接到数据实时回传，待中断当前命令和中断处理命令，接受一个字节超过最长等待时间,正常退出. #define RECIEVE 0xF0
     * //连续接受 F0 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传， //直到接受中断当前命令和中断处理命令。
     */
    static const uint8 RUNLINK = 0xF1; // 启动链路保持 F1
    // 启动链路保持，定时执行链路保持内容，在每次执行前回传链路保持开始信号，结束时回传链路保持结束信号。
    static const uint8 STOPLINK = 0xF2; // 中断链路保持 F2 结束链路保持执行。
    static const uint8 CLR_LINK = 0xDE; // 清除链路保持缓冲区
    static const uint8 DO_BAT_00 = 0xDF; // 批处理命令，执行一次命令缓冲区00地址的命令区
    // 3、多字节命令：（命令空间 = 0x30-= 0xCF）
    static const uint8 D0_BAT = 0x78; // 批处理命令，连续执行一次最多4块命令缓冲区的地址命令区；数据最多为4个命令区的首地址
    static const uint8 D0_BAT_FOR = 0x7C; // 批处理命令，连续执行无数次最多4块命令缓冲区的地址命令区；数据最多为4个命令区的首地址
    // 多字节命令
    static const uint8 SETTING = 0x80; // 下位机通讯链路状态字设定：设定3个通讯控制字，无用设定或没有设定都自动清零
    static const uint8 SETBAUD = 0x84; // 通讯波特率设定，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。

    /*
     * #define SETBYTETIME 0x88 //字节间时间设定 db20?（vpw为指令数）
     * ，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define SETWAITTIME 0x8c //空闲等待时间设定
     * db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define SETLINKTIME 0x90 //链路保持时建设定
     * db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define SETRECBBOUT 0x94
     * //接受字节超时错误判断 db20（vpw为指令数） ，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define
     * SETRECFROUT 0x98 //接受一帧超时错误判断?db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
     */
    static const uint8 Echo = 0x9C; // 回传指定数据，按序回传数据
    static const uint8 SETPORT0 = 0xA0; // 只有一个字节的数据，设定端口0
    static const uint8 SETPORT1 = 0xA4; // 只有一个字节的数据，设定端口1
    static const uint8 SETPORT2 = 0xA8; // 只有一个字节的数据，设定端口2
    static const uint8 SETPORT3 = 0xAC; // 只有一个字节的数据，设定端口3
    // 已删除#define SETALLPORT 0x6F //只有四个字节的数据，设定端口0，1，2，3
    static const uint8 SET_UPBAUD = 0xB0; //0x0C//0xB0; // 设置上位机的通讯波特率 ,仅有数据位1位,定义如下:其他非法

    /*
     * #define UP_9600BPS 0x00 #define UP_19200BPS 0x01 #define UP_38400BPS 0x02
     * #define UP_57600BPS 0x03 #define UP_115200BPS 0x04
     */
    static const uint8 DELAYSHORT = 0xB4; //0x78//0xB4; // 设定延时时间
    // (DB20)只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    static const uint8 DELAYTIME = 0xB8; //0x7C//0xB8; // 设定延时时间
    // (DB20)只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    static const uint8 DELAYLONG = 0xBC; // 设定延时时间 (DB200)
    // 只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    // Operat Buff CMD
    // 指定修改
    /*
     * #define UPDATE_1BYTE 0xc1 //81 结果地址 数据1 结果地址=数据1 #define UPDATE_2BYTE
     * 0xc3 //83 结果地址1 数据1 结果地址2 数据2 结果地址1=数据1 结果地址2=数据2 //数据拷贝 #define
     * COPY_DATA 0xcA //8A 结果地址1 操作地址1 长度 COPY 操作地址1 TO 结果地址1 FOR 长度 字节 //自增命令
     * #define DEC_DATA 0xc4 //84 结果地址 结果地址=结果地址-1 #define INC_DATA 0xc0 //80
     * 结果地址 结果地址=结果地址+1 #define INC_2DATA 0xc5 //85 结果地址1 结果地址2 结果地址1=结果地址1+1
     * 结果地址2=结果地址2+1 //加法命令 #define ADD_1BYTE 0xc2 //82 结果地址 操作地址1 数据1
     * 结果地址=操作地址1+数据1 #define ADD_2BYTE 0xc7 //87 结果地址1 结果地址2 数据1 数据2
     * 结果地址1=结果地址1+数据1 结果地址2=结果地址2+数据2 #define ADD_DATA 0xc6 //86 结果地址1 操作地址1
     * 操作地址2 结果地址1=操作地址1+操作地址2 //减法命令 #define SUB_DATA 0xce //8E 结果地址1 操作地址1
     * 操作地址2 结果地址1=操作地址1-操作地址2 #define SUB_BYTE 0xcD //8D 结果地址1 数据1
     * 结果地址1=数据1-结果地址1 #define INVERT_DATA 0xcC //8C 结果地址1 结果地址1=~结果地址 //取数据
     * #define GET_NDATA 0xc9 //88 地址 返回数据缓冲区指定的数据
     * 
     * #define UPDATE_1BYTE_A 0xc0 //81 结果地址 数据1 结果地址=数据1 #define UPDATE_2BYTE_A
     * 0xc0 //83 结果地址1 数据1 结果地址2 数据2 结果地址1=数据1 结果地址2=数据2 //自增命令 #define
     * DEC_DATA_A 0xc4 //84 结果地址 结果地址=结果地址-1 #define INC_DATA_A 0xc0 //80 结果地址
     * 结果地址=结果地址+1 #define INC_2DATA_A 0xc4 //85 结果地址1 结果地址2 结果地址1=结果地址1+1
     * 结果地址2=结果地址2+1 //加法命令 #define ADD_1BYTE_A 0xc0 //82 结果地址 操作地址1 数据1
     * 结果地址=操作地址1+数据1 #define ADD_2BYTE_A 0xc4 //87 结果地址1 结果地址2 数据1 数据2
     * 结果地址1=结果地址1+数据1 结果地址2=结果地址2+数据2 #define ADD_DATA_A 0xc4 //86 结果地址1 操作地址1
     * 操作地址2 结果地址1=操作地址1+操作地址2 //减法命令 #define SUB_DATA_A 0xcc //8E 结果地址1 操作地址1
     * 操作地址2 结果地址1=操作地址1-操作地址2 #define SUB_BYTE_A 0xcc //8D 结果地址1 数据1
     * 结果地址1=数据1-结果地址1 #define INVERT_DATA_A 0xcC //8C 结果地址1 结果地址1=~结果地址
     */
    // 取数据
    static const uint8 GET_DATA = 0xC8; // 88 地址 返回数据缓冲区指定的数据
    /***************************************************************************
     * 返回命令定义区: 返回命令分为两类: 1 单字节返回:无长度和校验,仅返回单字节 1 错误,成功信息: 2 接受的数据:(接受数据,通讯端口数据)
     * 使用于缓冲区命令 2 多字节返回: 1 格式:接受的命令字 + 长度 + 数据 + 校验和 长度：仅包含数据个数 使用于非缓冲区命令 3
     * 中断命令不返回：以执行结果返回
     ***************************************************************************/
    /*
     * // 1 单字节返回:无长度和校验,仅返回单字节 //接受返回错误信息定义 #define UP_TIMEOUT 0xC0 //接受命令超时错误
     * #define UP_DATAEER 0xC1 //接受命令数据错误 #define OVER_BUFF 0xC2
     * //批处理缓冲区溢出,不判断链路保持数据是否会破坏缓冲区数据, //仅判断数据长度+数据地址>链路保持的开始位置成立溢出. #define
     * ERROR_REC 0xC3 //其他接受错误
     * 
     * //执行操作错误 #define SUCCESS 0xAA //执行成功 #define RUN_ERR 0xC4 //运行启动检测错误
     */
    // /////////////////////////////////////////////////////
    // 时间计算设置Commbox宏定义区
    // //////////////////////////////////////////////////////
    static const uint32 REPLAYTIMES = 3; // 错误运行次数
    // /////////////////////////////////////////////////////
    // 设置Commbox宏定义区
    // //////////////////////////////////////////////////////
    static const uint32 TIMEVALUE = 1000000; // 万分之一秒微妙
    static const uint32 COMMBOXINFOLEN = 18; // 共有18个数据需从COMMBOX得到
    static const uint32 VERSIONLEN = 2;
    static const uint32 MINITIMELEN = 3;
    static const uint32 COMMBOXPORTNUM = 4;
    static const uint32 COMMBOXIDLEN = 10;
    //    static const uint32 LINKBLOCK = MAXIM_BLOCK; // 链路保持的命令缓冲区
    // 批处理执行次数
    static const uint8 RUN_ONCE = 0x00;
    static const uint8 RUN_MORE = 0x01;
    // 通讯校验和方式
    static const uint8 CHECK_SUM = 0x01;
    static const uint8 CHECK_REVSUM = 0x02;
    static const uint8 CHECK_CRC = 0x03;
    // RF多对一的设定接口
    static const uint8 SETDTR_L = 0x02;
    static const uint8 SETDTR_H = 0x03;
    static const uint8 MAX_RFADD = 0x2F; // = 0x00-= 0x2F间的= 0x30个地址
    static const uint8 SETADD = 0x10; // 切换无线通讯设备到新地址
    static const uint8 CHANGEADD = 0x40; // 改变当前与之通讯的无线设备的地址
    static const uint8 SETRFBAUD = 0x04; // 改变无线串口通讯波特率
    static const uint8 RESET_RF = 0x00; // 复位无线通讯主设备，该命令需在9600波特率下实现
    // /////////////////////////////////////////////////////////////////////////////
    // 通讯口 PORT
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 DH = 0x80; // 高电平输出,1为关闭,0为打开
    static const uint8 DL2 = 0x40; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线
    static const uint8 DL1 = 0x20; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制
    static const uint8 DL0 = 0x10; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制
    static const uint8 PWMS = 0x08; // PWM发送线
    static const uint8 PWMR = 0x04;
    static const uint8 COMS = 0x02; // 标准发送通讯线路
    static const uint8 COMR = 0x01;
    static const uint8 SET_NULL = 0x00; // 不选择任何设置
    // /////////////////////////////////////////////////////////////////////////////
    // 通讯物理控制口
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 PWC = 0x80; // 通讯电平控制,1为5伏,0为12伏
    static const uint8 REFC = 0x40; // 通讯比较电平控制,1为通讯电平1/5,0为比较电平控制1/2
    static const uint8 CK = 0x20; // K线控制开关,1为双线通讯,0为单线通讯
    static const uint8 SZFC = 0x10; // 发送逻辑控制,1为负逻辑,0为正逻辑
    static const uint8 RZFC = 0x08; // 接受逻辑控制,1为负逻辑,0为正逻辑
    static const uint8 DLC0 = 0x04; // DLC1接受控制,1为接受关闭,0为接受打开
    static const uint8 DLC1 = 0x02; // DLC0接受控制,1为接受关闭,0为接受打开
    static const uint8 SLC = 0x01; // 线选地址锁存器控制线(待用)
    static const uint8 CLOSEALL = 0x08; // 关闭所有发送口线，和接受口线
    // /////////////////////////////////////////////////////////////////////////////
    // 通讯控制字1设定
    // /////////////////////////////////////////////////////////////////////////////
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
    // /////////////////////////////////////////////////////////////////////////////
    // 通讯控制字3设定
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 ONEBYONE = 0x80;
    static const uint8 INVERTBYTE = 0x40;
    static const uint8 ORIGINALBYTE = 0x00;
    // /////////////////////////////////////////////////////////////////////////////
    // 通讯设置参数时间
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 SETBYTETIME = 0x88; // 字节间时间设定 db20?
    // ，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    static const uint8 SETVPWSTART = 0x08; // 设置vpw发送数据时需发送0的时间。
    static const uint8 SETWAITTIME = 0x8C; // 空闲等待时间设定
    // db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    static const uint8 SETLINKTIME = 0x90; // 链路保持时建设定
    // db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    static const uint8 SETRECBBOUT = 0x94; // 接受字节超时错误判断 db20（vpw为指令数）
    // ，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    static const uint8 SETRECFROUT = 0x98; // 接受一帧超时错误判断?db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
    static const uint8 SETVPWRECS = 0x14;
    // /////////////////////////////////////////////////////////////////////////////
    // 上下位机通讯波特率
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 UP_9600BPS = 0x0;
    static const uint8 UP_19200BPS = 0x01;
    static const uint8 UP_38400BPS = 0x02;
    static const uint8 UP_57600BPS = 0x03;
    static const uint8 UP_115200BPS = 0x04;
    // /////////////////////////////////////////////////////////////////////////////
    // 操作数据缓冲区
    // /////////////////////////////////////////////////////////////////////////////
    // 数据拷贝
    static const uint8 COPY_DATA = 0xCA; // 8A 结果地址1 操作地址1 长度 COPY 操作地址1 TO 结果地址1
    // FOR 长度 字节
    // 修改数据
    static const uint8 UPDATE_1BYTE = 0xC1; // 81 结果地址 数据1 结果地址=数据1
    static const uint8 UPDATE_2BYTE = 0xC3; // 83 结果地址1 数据1 结果地址2 数据2 结果地址1=数据1
    // 结果地址2=数据2
    // 自增命令
    static const uint8 DEC_DATA = 0xC4; // 84 结果地址 结果地址=结果地址-1
    static const uint8 INC_DATA = 0xC0; // 80 结果地址 结果地址=结果地址+1
    static const uint8 INC_2DATA = 0xC5; // 85 结果地址1 结果地址2 结果地址1=结果地址1+1
    // 结果地址2=结果地址2+1
    // 加法命令
    static const uint8 ADD_1BYTE = 0xC2; // 82 结果地址 操作地址1 数据1 结果地址=操作地址1+数据1
    static const uint8 ADD_2BYTE = 0xC7; // 87 结果地址1 数据1 结果地址2 数据2 结果地址1=结果地址1+数据1
    // 结果地址2=结果地址2+数据2
    static const uint8 ADD_DATA = 0xC6; // 86 结果地址1 操作地址1 操作地址2 结果地址1=操作地址1+操作地址2
    // 减法命令
    static const uint8 SUB_DATA = 0xCE; // 8E 结果地址1 操作地址1 操作地址2 结果地址1=操作地址1-操作地址2
    static const uint8 SUB_BYTE = 0xCD; //0xB0//0xCD; // 8D 结果地址1 数据1 结果地址1=数据1-结果地址1
    static const uint8 INVERT_DATA = 0xCC; // 8C 结果地址1 结果地址1=~结果地址
    // /////////////////////////////////////////////////////////////////////////////
    // 接受命令类型定义
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 GET_PORT1 = 0xD8; //0x5C//0xD8; // 等到通讯口的当前状态
    static const uint8 SET55_BAUD = 0xDA; //0x44//0xDA; // 计算= 0x55的波特率
    //    static const uint8 REC_FR = 0xE0; // 接受一帧命令 E0 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传，
    //    static const uint8 REC_LEN_1 = 0xE1; // 接受1个数据，返回
    //    static const uint8 REC_LEN_2 = 0xE2; // 接受2个数据，返回
    //    static const uint8 REC_LEN_3 = 0xE3; // 接受3个数据，返回
    //    static const uint8 REC_LEN_4 = 0xE4; // 接受4个数据，返回
    //    static const uint8 REC_LEN_5 = 0xE5; // 接受5个数据，返回
    //    static const uint8 REC_LEN_6 = 0xE6; // 接受6个数据，返回
    //    static const uint8 REC_LEN_7 = 0xE7; // 接受7个数据，返回
    //    static const uint8 REC_LEN_8 = 0xE8; // 接受8个数据，返回
    //    static const uint8 REC_LEN_9 = 0xE9; // 接受9个数据，返回
    //    static const uint8 REC_LEN_10 = 0xEA; // 接受10个数据，返回
    //    static const uint8 REC_LEN_11 = 0xEB; // 接受11个数据，返回
    //    static const uint8 REC_LEN_12 = 0xEC; // 接受12个数据，返回
    //    static const uint8 REC_LEN_13 = 0xED; // 接受13个数据，返回
    //    static const uint8 REC_LEN_14 = 0xEE; // 接受14个数据，返回
    //    static const uint8 REC_LEN_15 = 0xEF; // 接受15个数据，返回
    //    static const uint8 RECEIVE = 0xF0; // 连续接受 F0 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传，
    // /////////////////////////////////////////////////////////////////////////////
    // ComBox记录信息和当前状态种类定义
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 GET_TIME = 0xF5; //0x18//0xF5; // 得到时间设定 DD
    // 返回字节时间、等待发送时间、链路保持时间、字节超时时间、接受超时时间
    static const uint8 GET_SET = 0xF6; //0x1C//0xF6; // 得到链路设定 DE 返回链路控制字(3字节)、通讯波特率
    static const uint8 GET_PORT = 0xF7; //0x2C//0xF7; // 得到端口设置 DF 返回端口p0，p1，p2，p3
    static const uint8 GET_LINKDATA = 0xF8; // 得到链路数据 FC 返回链路保持命令块中的所有内容 (中断命令)
    static const uint8 GET_BUFFDATA = 0xF9; // 得到缓冲器数据 FD 返回整个缓冲区数据 (中断命令)
    static const uint8 GET_COMMAND = 0xFA; // 得到命令数据 FE 返回上一执行命令。 (中断命令)
    // /////////////////////////////////////////////////////////////////////////////
    // 返回失败时，可根据Error_Record的值查找错误表定义
    // /////////////////////////////////////////////////////////////////////////////
    static const uint8 ILLIGICAL_LEN = 0xFF; // 设置命令数据非法超长
    static const uint8 NOBUFF_TOSEND = 0xFE; // 无交换缓冲区用于发送数据存放
    static const uint8 SENDDATA_ERROR = 0xFD; // 上位机发送数据异常
    static const uint8 CHECKSUM_ERROR = 0xFC; // 接受命令回复校验和出错
    static const uint8 TIMEOUT_ERROR = 0xFB; // 接受数据超时错误
    static const uint8 LOST_VERSIONDATA = 0xFA; // 读到的Commbox数据长度不够.
    static const uint8 ILLIGICAL_CMD = 0xF9; // 无此操作功能,没有定义.
    static const uint8 DISCONNECT_COMM = 0xF8; //2//0xF8; // 没有连接上串口
    static const uint8 DISCONNECT_COMMBOX = 0xF7; //3//0xF7; // 没有连接上COMMBOX设备
    static const uint8 NODEFINE_BUFF = 0xF6; // 没有此命令块存在,未定义
    static const uint8 APPLICATION_NOW = 0xF5; // 现有缓冲区申请,未取消,不能再此申请
    static const uint8 BUFFBUSING = 0xF4; // 此缓冲区有数据未被撤销,不能使用,需删除此缓冲区,方可使用
    static const uint8 BUFFFLOW = 0xF3; // 整个缓冲区无可使用的空间,不能申请,需删除缓冲区释放空间,方可使用
    static const uint8 NOAPPLICATBUFF = 0xF2; // 未申请错误,需先申请,方可使用
    static const uint8 UNBUFF_CMD = 0xF1; // 不是缓冲区命令,不能加载
    static const uint8 NOUSED_BUFF = 0xF0; // 该缓冲区现没有使用,删除无效
    static const uint8 KEEPLINK_ERROR = 0xEF; // 链路保持已断线
    static const uint8 UNDEFINE_CMD = 0xEE; // 无效命令,未曾定义
    static const uint8 UNSET_EXRSBIT = 0xED; // 没有设定扩展RS232的接受数据位个数
    static const uint8 COMMBAUD_OUT = 0xEC; // 按照定义和倍增标志计算通讯波特率超出范围
    static const uint8 COMMTIME_OUT = 0xEB; // 按照定义和倍增标志计算通讯时间超出范围
    static const uint8 OUTADDINBUFF = 0xEA; // 缓冲区寻址越界
    static const uint8 COMMTIME_ZERO = 0xE9; // commbox时间基数为零
    static const uint8 SETTIME_ERROR = 0xE8; // 延时时间为零
    static const uint8 NOADDDATA = 0xE7; // 没有向申请的缓冲区填入命令,申请的缓冲区被撤销
    static const uint8 TESTNOLINK = 0xE6; // 选择的线路没有连通
    static const uint8 PORTLEVELIDLE = 0xE5; // 端口电平为常态
    static const uint8 COMMBOXID_ERR = 0xE4; //1//0xE4; // COMMBOX ID错误
    static const uint8 UP_TIMEOUT = 0xC0; // COMMBOX接受命令超时错误
    static const uint8 UP_DATAEER = 0xC1; // COMMBOX接受命令数据错误
    static const uint8 OVER_BUFF = 0xC2; // COMMBOX批处理缓冲区溢出,不判断链路保持数据是否会破坏缓冲区数据,
    // 仅判断数据长度+数据地址>链路保持的开始位置成立溢出.
    static const uint8 ERROR_REC = 0xC3; // COMMBOX其他接受错误
    // COMMBOX执行操作错误
    static const uint8 SUCCESS = 0xAA; // COMMBOX执行成功
    static const uint8 SEND_OK = 0x55;
    static const uint8 RF_ERR = 0xC8;
    static const uint8 RUN_ERR = 0xC4; // COMMBOX运行启动检测错误
    // 通讯通道定义
    static const uint8 SK0 = 0;
    static const uint8 SK1 = 1;
    static const uint8 SK2 = 2;
    static const uint8 SK3 = 3;
    static const uint8 SK4 = 4;
    static const uint8 SK5 = 5;
    static const uint8 SK6 = 6;
    static const uint8 SK7 = 7;
    static const uint8 SK_NO = 0xFF;
    static const uint8 RK0 = 0;
    static const uint8 RK1 = 1;
    static const uint8 RK2 = 2;
    static const uint8 RK3 = 3;
    static const uint8 RK4 = 4;
    static const uint8 RK5 = 5;
    static const uint8 RK6 = 6;
    static const uint8 RK7 = 7;
    static const uint8 RK_NO = 0xFF;
    static const uint8 SEND_CMD = 0x01;
private:
    //////////////////////////////////////////////////////////////////////
    //维护COMMBOX数据缓冲区的数据结构	CMDBUFFINFOSTRUCT
    //组成:
    //BYTE CMDBuffID = 0;					//当前登记的命令区的索引
    //BYTE CMDB_UsedNum=0;				//在命令缓冲区已使用的命令区地址连续纪录数
    //BYTE CMDBuff_ADD[MAXIM_BLOCK+2];	//命令缓冲区地址
    //BYTE CMDBuff_Used[MAXIM_BLOCK];		//已使用的命令区连续纪录
    //////////////////////////////////////////////////////////////////////

    struct v1_buffer {
        uint8 id;
        uint32 used_num;
        uint8 add[MAXIM_BLOCK + 2];
        uint8 used[MAXIM_BLOCK];

        v1_buffer()
        : id(0)
        , used_num(0) {

        }

        uint32 size() {
            return add[v1_shared::LINKBLOCK] - add[SWAPBLOCK];
        }
    };

    struct v1_information {
        uint32 time_unit;
        uint32 time_base_db;
        uint32 time_extern_db;
        uint32 cmd_buff_len;
        uint8 version[VERSIONLEN];
        uint8 id[COMMBOXIDLEN];
        uint8 port[COMMBOXPORTNUM];
        uint8 head_password;

        v1_information()
        : time_unit(0)
        , time_base_db(0)
        , time_extern_db(0)
        , cmd_buff_len(0)
        , head_password(0) {

        }
    };
private:
    int32 _serial_port_baud;
    uint8 _serial_port_databits;
    int32 _serial_port_stopbits;
    int32 _serial_port_flow_control;
    int32 _serial_port_parity;
    v1_buffer _buffer;
    v1_information _info;
    uint8 _cmd_temp[256]; //写入命令缓冲区
    bool _is_initialize;
public:
    bool serial_port_need_config();
    int32 serial_port_baud();
    uint8 serial_port_databits();
    int32 serial_port_parity();
    int32 serial_port_stopbits();
    int32 serial_port_flow_control();

    v1_c168_box(const boost::shared_ptr<port> &p, const boost::shared_ptr<v1_shared> &s);
    // ///////////////////////////////////////////////////////////////////////////
    // 全局函数：SendOk()
    //
    // 返回值： bool : 执行成功接纳1,否则接纳0
    // 功能：
    // 检查并发送出去的命令是否被接纳。
    // ///////////////////////////////////////////////////////////////////////////
    bool send_ok(int64 microseconds);
    // /////////////////////////////////////////////////////////////////
    // 全局函数 bool CheckResult(LONG m_Time)
    //
    // 返回值: bool : 执行成功返回1,否则返回0
    // 参数 : m_Time 超时错误时间
    // 功能:
    // 1.在指定的时间内读取成功标志。
    // 2.返回成功结果。
    // /////////////////////////////////////////////////////////////////
    bool check_result(int64 microseconds);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // bool StopNow(bool IsStopExcute)
    //
    // 返回值: bool ：成功返回1，失败返回0。
    // 参数: 一个：
    // bool IsStopExcute :
    // 功能: 当IsStopExcute为真，停止Commbox当前执行的操作，
    // 否则，停止当前接受数据，
    // ////////////////////////////////////////////////////////////////////
    bool stop_now(bool is_stop_execute);
    // /////////////////////////////////////////////////////////////////
    // 全局函数 :
    // WORD ReadData(WORD length,BYTE * recivebuffer, LONG m_Time,INT RecBBTime)
    //
    // 返回: 返回得到的数据长度,或超时错误00
    // 参数 :
    // INT length 指定接收数据长度,当指定的长度为0时，以时间间隔读取数据。
    // recivebuffer 接收数据缓冲区
    // m_Time 超时错误时间
    // RecBBTime 读取数据的字节时间间隔大于此时间为结束。
    // 注意CheckResult的处理，适合与接受命令在批处理最后位置。
    // 功能:
    // 1.在指定的时间内读取数据
    // 2.在指定时间内读完数据则退出
    // 3.超出指定时间则退出
    // /////////////////////////////////////////////////////////////////
    int32 read_data(uint8 *buff, int32 offset, int32 count, int64 microseconds);
    // for serial port
    bool serial_port_change_config();
    bool check_serial_port_change_config();
    bool open_comm();
    // ///////////////////////////////////////////////////////////////////
    // 全局函数 :
    // bool CloseComm()
    //
    // 返回值: bool : 执行成功返回1,否则返回0
    // 参数 : 无
    // 功能: 复位Commbox,停止链路保持，复位校验，关闭串口
    // /////////////////////////////////////////////////////////////////
    bool close_comm();
    // ////////////////////////////////////////////////////////////////////
    // 全局函数: bool NewBatch(BYTE BuffID)
    // 申请一个新的数据缓冲区块,并指定其标识
    // 参数: BYTE BuffID： 指定其标识，
    // 返回: 下一命令的块内地址
    // 功能:
    // 0.数据块标识登记是否有其他申请位清除?
    // 1.检查申请的数据块标识是否已存在?
    // 2.检查申请的数据块标识是否在指定的范围内?
    // 3.检查成功,填写CMD_Temp的命令及校验和
    // 4.登记写入的数据块标识.
    // 5.仅处理链路保持区和命令缓冲区,不处理交换区
    // ////////////////////////////////////////////////////////////////////
    bool new_batch(uint8 buff_id);
    // ////////////////////////////////////////////////////////////////////
    // 静态函数:
    // BYTE AddToBuff(BYTE commandword ,BYTE length,char * data)
    // 向已申请的新数据缓冲区块填入命令
    //
    // 返回: 下一命令的块内地址
    // 参数: commandword 操作命令字
    // length 要写入数据的长度
    // data 写入的数据
    // 功能:
    // 0.数据块标识登记是否有申请?
    // 1.检查是否有足够的空间存储?
    // 2.检查命令的合法性,是否为缓冲区命令?
    // 3.写入命令
    // 4.计算校验
    // ////////////////////////////////////////////////////////////////////
    bool add_to_buff(uint8 command_word, int32 offset, int32 count, const uint8 *data);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:bool EndBatch()
    //
    // 返回: bool : 成功返回1，失败返回0
    // 参数: 无
    // 功能:
    // 0.数据块标识登记是否有申请?
    // 1.发送命令到Commbox
    // 2.接受结果信息
    // 3.如超时接到数据,或返回错误信息
    // 4.修改命令缓冲区信息
    // 5.清除登记
    // ////////////////////////////////////////////////////////////////////
    bool end_batch();
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // bool DelBatch(BYTE BuffID)
    //
    // 参数: BYTE BuffID 指出要删除的命令缓存标号
    // 返回: 成功返回1，失败返回0。
    // 功能: 删除的指定已使用命令缓存块
    // 0.缓冲区数据块不存在，返回错误
    // 1.是否缓冲区数据块正在申请，则删除正常退出。
    // 2.复位申请缓冲区记录，复位申请缓冲区的地址。
    // ////////////////////////////////////////////////////////////////////
    bool del_batch(uint8 buff_id);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE SetLineLevel(BYTE ValueLow , BYTE ValueHigh)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 四个：
    // BYTE ValueLow :指出要设低的断口
    // BYTE ValueHigh:指出要设高的断口
    // 功能: 设置通讯口线的电平位置，
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool set_line_level(uint8 value_low, uint8 value_high);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE SetCommCtrl(BYTE ValueOpen , BYTE ValueClose )
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 四个：
    // BYTE ValueOpen :指出要打开的开关
    // BYTE ValueClose:指出要关闭的开关
    // 功能: 设置通讯物理曾的位置，
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool set_comm_ctrl(uint8 value_open, uint8 value_close);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE SetCommLine(BYTE SendLine , BYTE RecLine)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 四个：
    // BYTE SendLine :指出要发送的数据断口
    // BYTE RecLine :指出要接受的数据断口
    // 功能: 设置通讯线路的断口
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool set_comm_line(uint8 send_line, uint8 recv_line);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE TurnOverOneByOne()
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 两个：
    // 功能: 设置通讯链路为标准下的，翻转OneByOne标志
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool turn_over_one_by_one();
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE SetEchoData(BYTE EchoLen , BYTE * EchoBuff)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 四个：
    // BYTE EchoLen :回传数据长度
    // BYTE * EchoBuff :回传数据缓存
    // 功能: 设置执行将EchoBuff中的数值原样返回。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool set_echo_data(uint8 *echo_buff, int32 echo_len);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE KeepLink(bool IsRunLink)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 三个：
    // bool IsRunLink :启动链路保持设1，停止链路保持为0
    // 功能: 设置启动或停止链路保持。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool keep_link(bool is_run_link);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE SetCommLink(BYTE CtrlWord1,BYTE CtrlWord2,BYTE CtrlWord3)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 五个：
    // BYTE CtrlWord1 :通讯方式设定
    // #define RS_232 0x00 //标准RS232方式
    // #define EXRS_232 0x20 //扩展RS232方式
    // #define SET_VPW 0x40 //vpw方式
    // #define SET_PWM 0x60 //pwm方式
    // #define BIT9_SPACE 0x00 //标准RS232方式的校验位为0
    // #define BIT9_MARK 0x01 //标准RS232方式的校验位为1
    // #define BIT9_EVEN 0x02 //标准RS232方式的校验位为偶校验
    // #define BIT9_ODD 0x03 //标准RS232方式的校验位为寄校验
    // #define SEL_SL 0x00 //选择标准通讯通道
    // #define SEL_DL0 0x08 //选择DL0
    // #define SEL_DL1 0x10 //选择DL1
    // #define SEL_DL2 0x18 //选择DL2
    // #define SET_DB20 0x04 //设置通讯波特率为DB20方式
    // #define UN_DB20 0x00 //设置通讯波特率为正常方式
    //
    // BYTE CtrlWord2 :在扩展RS232方式下，为数据位的个数，在标准RS232方式下
    // 为接受一怔数据的长度偏移
    // BYTE CtrlWord3 :在标准RS232方式下，为长度所在接受的位置，将该位置的数值同CtrlWord2
    // 相加，表示以后要接的数据长度。
    // #define ONEBYONE 0x80 //在标准RS232方式下，设定发一个接一个链路
    // #define INVERTBYTE 0x40 //在标准RS232方式下，设定接一个取反发回
    // #define ORIGNALBYTE 0X00 //在标准RS232方式下，设定接一个原样发回
    // 若在标准RS232方式下，CtrlWord3的长度所在接受的位置为零，则按BBTIMEOUT超时取帧。
    // 功能: 设置启动或停止链路保持。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool set_comm_link(uint8 ctrl_word1, uint8 ctrl_word2, uint8 word3);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE SetCommBaud(DWORD Baud)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 三个：
    // DWORD Baud :设定波特率
    // 功能: 设置启动或停止链路保持。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool set_comm_baud(double baud);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE SetCommTime(BYTE Type,DWORD Time)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 三个：
    // BYTE Type :设定时间类型
    // DWORD Time :设定时间值
    // 功能: 设置各种通讯时间。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool set_comm_time(uint8 type, uint32 time);
    // ////////////////////////////////////////////////////////////////////
    // 全局通讯函数:
    // BYTE RunReceive(BYTE Type)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 三个：
    // BYTE Type :设定接受数据类型
    // 功能: 设置各种通讯时间，凡为通讯函数，都要在读取数据后判断操作是否完成。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool run_receive(uint8 type);
    // ////////////////////////////////////////////////////////////////////
    // 静态函数:
    // bool GetAstractAdd(BYTE BuffID ,BYTE * BuffAdd)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。
    // 参数: 两个：
    // BYTE BuffID :指定块标号
    // BYTE * BuffAdd :指定块内地址，返回绝对地址
    // 功能: 得到缓存中的绝对地址
    // ////////////////////////////////////////////////////////////////////
    uint8 get_abs_add(uint8 buff_id, uint8 add);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // BYTE UpdateBuff(BYTE Type, BYTE * Buffer)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 四个：
    // BYTE Type :对缓冲区操作的类型
    // BYTE * Buffer :操作的数据和地址，地址有2个字节表示，低字节表示块，高字节表示块内地址
    // 功能: 修改数据缓存取，详见各个类型的功能
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool update_buff(uint8 type, uint8 *buffer);
    // ////////////////////////////////////////////////////////////////////
    // 全局通讯函数:
    // BYTE CommboxDelay(DWORD Time)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 四个：
    // DWORD Time :设置下位机延时时间以微秒单位
    // 功能: 使用后，需判断是否执行结束。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool commbox_delay(uint32 time);
    // ////////////////////////////////////////////////////////////////////
    // 全局通讯函数:
    // BYTE SendOutData(BYTE length,BYTE * Buffer)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。在写入缓存时，成功返回下调指令存放位置
    // 参数: 四个：
    // BYTE length :发送数据长度
    // BYTE * Buffer :发送数据缓存
    // 功能: 使用后，需判断是否执行结束。
    // 如要立即执行，则返回执行结果，如写入缓冲区，返回下条命令的地址
    // ////////////////////////////////////////////////////////////////////
    bool send_out_data(const uint8 *buffer, int32 offset, int32 count);
    // ////////////////////////////////////////////////////////////////////
    // 全局通讯函数:
    // bool RunBatch(BYTE Lenght , BYTE *BuffID , bool IsExcutManyTime)
    //
    // 返回值: BYTE ：成功返回1，失败返回0。
    // 参数: 三个：
    // BYTE length :执行命令块的个数
    // BYTE * Buffer :执行命令块的标识，按执行的先后顺序存放的缓存
    // bool IsExcutManyTime :须不断循环执行吗
    // 功能: 使用后，需判断是否执行结束。
    // ////////////////////////////////////////////////////////////////////
    bool run_batch(uint8 *buff_id, int32 count, bool is_execute_many);
    int32 read_bytes(uint8 *buff, int32 offset, int32 count);
private:
    // ///////////////////////////////////////////////////////////////////////////
    // 静态函数：bool CheckIdle()
    //
    // 返回值： bool : 执行成功返回1,否则返回0
    // 参数： 无
    // 功能：
    // 检查当前Commbox是否在作链路保持工作。
    // a.读取接受缓存中最后一个数据。
    // b.是否为成功标志？成功则退出
    // c.否则在2/3倍的链路保持时间中，查找成功标志？成功则退出
    // d.否则失败。
    // 失败原因:
    // a.链路保持通讯失败，需发停止命令，重新尝试,失败需复位
    // b.数据接受缓存在接受时未检查结束
    // ///////////////////////////////////////////////////////////////////////////
    bool check_idle();
    uint32 get_box_ver();
    // ///////////////////////////////////////////////////////////////////////////
    // 全局函数：bool CommboxDo(BYTE commandword,BYTE length,BYTE * buff)
    //
    // 返回值： bool : 执行成功返回1,否则返回0
    // 参数： 三个BYTE commandword,BYTE length,BYTE * buff
    // BYTE commandword: 要commbox做事的命令字
    // BYTE length : 命令参数信息
    // BYTE * buff : 参数缓存
    // 功能：
    // 检查Commbox是否空闲，根据参数组合，并发送出去。
    // a.检查Commbox是否空闲，否，则返回0。
    // b.发送命令，成功则退出
    // 失败原因:
    // a.检查链路保持通讯失败。
    // b.计算机通讯串口有错误，需提示操作。
    // c.不符合命令格式和定义。
    // d.在发送转发命令时错误。
    // ///////////////////////////////////////////////////////////////////////////
    bool commbox_do(uint8 command_word, int32 offset, int32 count, const uint8 *buff);
    bool send_data_to_ecu(uint8 command_word, int32 offset, int32 count, const uint8 *buff);
    bool send_data_to_ecu_new(uint8 command_word, int32 offset, int32 count, const uint8 *buff);
    bool send_data_to_ecu_old(uint8 command_word, int32 offset, int32 count, const uint8 *buff);
    bool commbox_command(uint8 command_word, int32 offset, int32 count, const uint8 *buff);
    bool do_set(uint8 command_word, int32 offset, int32 count, const uint8 *buff);
    // /////////////////////////////////////////////////////////////////
    // 全局函数 GetCMDData(BYTE command,BYTE * recivebuffer)
    //
    // 返回值: BYTE : 执行成功返回1,否则返回0
    // 参数 : BYTE command 执行命令的类型
    // recivebuffer 接收数据缓冲区
    // 功能:
    // 1.在一定的时间内读取指定命令的数据
    // 2.检查校验和
    // 3.如在指定时间内未能读完数据则错误退出
    // 4.未能读到指定命令的数据错误退出
    // /////////////////////////////////////////////////////////////////
    int32 get_cmd_data(uint8 command, uint8 *receive_buffer);
    // ////////////////////////////////////////////////////////////////////
    // 通讯串口和Commbox初始化设定
    // 并进行握手通信，以能正常通讯
    // 关闭串口复位Commbox.
    // ////////////////////////////////////////////////////////////////////
    // /////////////////////////////////////////////////////////////////
    // 静态函数 :
    // bool CheckBox()
    //
    // 返回值: bool : 执行成功返回1,否则返回0
    // 参数 : 无
    // 功能:
    // 1.产生随机数据，校验CommBox合法性
    // 2.合法正常退出，否则异常错误。
    // /////////////////////////////////////////////////////////////////
    bool check_box();
    // ///////////////////////////////////////////////////////////////////
    // 静态函数 :
    // bool InitBox()
    //
    // 返回值: bool : 执行成功返回1,否则返回0
    // 参数 : 无
    // 功能:
    // 1.读取Commbox相关信息，并计算赋值
    // 2.对通讯参数初始化
    // 3.调用CheckBox()检查。
    // /////////////////////////////////////////////////////////////////
    bool init_box();
    // ///////////////////////////////////////////////////////////////////
    // 全局函数 :
    // bool SetPCBaud(BYTE Baud)
    //
    // 返回值: bool : 执行成功返回1,否则返回0
    // 参数 : 一个,有宏定义指明参数
    // BYTE Baud:上位同Commbox通讯速率
    // #define UP_9600BPS 0x00 9600bps
    // #define UP_19200BPS 0x01 19200bps
    // #define UP_38400BPS 0x02 38400bps
    // #define UP_57600BPS 0x03 57600bps
    // #define UP_115200BPS 0x04 115200bps
    // 功能: 用于切换通讯的波特率
    // /////////////////////////////////////////////////////////////////
    // ///////////////////////////////////////////////////////////////////
    // 全局函数 :
    // bool OpenComm(BYTE Port)
    //
    // 返回值: bool : 执行成功返回1,否则返回0
    // 参数 : 无
    // 功能: 打开串口，设置通讯时间，初始化Commbox
    // 1.设置通讯时间
    // 2.打开指定串口，设置波特率为START_BAUD
    // 3.调用CheckBox()检查。
    // /////////////////////////////////////////////////////////////////
    bool send_to_box(uint8 command_word, int32 offset, int32 count, const uint8 *buff);
    void get_link_time(uint8 type, uint32 time);
    // ////////////////////////////////////////////////////////////////////
    // 全局函数:
    // SetRF(BYTE CMD,BYTE CMDInfo)
    //
    // 返回值: bool ：返回值 0:通讯错误或不连通
    // 1:是连通的
    // 参数: 两个
    // BYTE CMD, 设定命令
    // BYTE CMDInfo, 命令信息
    // 功能: 1.用以设定无线通讯的参数
    // ////////////////////////////////////////////////////////////////////
    bool set_rf(uint8 cmd, uint8 cmd_info);
};


}


#endif	/* JM_V1_C168_BOX_HPP */

