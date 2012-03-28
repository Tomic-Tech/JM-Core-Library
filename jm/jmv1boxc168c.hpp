#ifndef __JM_V1_BOX_C168_C_HPP__
#define __JM_V1_BOX_C168_C_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <boost/cstdint.hpp>

namespace JM
{
    namespace V1
    {
        namespace C168
        {
            class Constant
            {
            public:
                //////////////////////////////////////////////////////////////////////////
                // CommBox 固定信息 宏定义表
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t NULLADD = 0xFF; // 表示此块无使用
                static const boost::uint8_t MAXIM_BLOCK = 0x40; // 命令缓从区的最大数
                static const boost::uint8_t SWAPBLOCK = (MAXIM_BLOCK + 1); // 数据交换区的块表识
                static const boost::uint32_t START_BAUD = 57600; // 上位机同下位机通信在复位或上电时波特率为57600
                static const boost::uint8_t CMD_DATALEN = 4; // 非发送命令最大长度
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
                static const boost::uint8_t WR_DATA = 0xD0; // 写缓冲区命令字,写入数据到命令缓冲区
                static const boost::uint8_t WR_LINK = 0xFF; // 若写入命令的地址为WR_LINK，写入数据到链路保持区链路保持区存放在命令缓冲区最后,存放次序:按地址从低到高
                static const boost::uint8_t SEND_LEN = 0xA8; // 一次发送数据的数据长度,0X70个数据
                static const boost::uint8_t SEND_DATA = 0x00;
                // 2、单字节命令：（大于写入命令缓冲区命令字 WR_DATA 0xD0，皆为单字节命令区）
                // 非缓冲区命令
                static const boost::uint8_t RESET = 0xF3; // 软件复位命令 清除所有缓冲区和寄存器内容。
                static const boost::uint8_t GETINFO = 0xF4;
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
                static const boost::uint8_t STOP_REC = 0xFB; // 中断接受命令 强行退出当前接受命令，不返回错误。(中断命令)
                static const boost::uint8_t STOP_EXECUTE = 0xFC; // 中断批处理命令
                // 在当前执行时，通过该命令停止当前接受操作，返回错误。(中断命令)
                // 单字节缓冲区命令
                // static const boost::uint8_t GET_PORT1 0xD8 //等到通讯口的当前状态
                static const boost::uint8_t SET_ONEBYONE = 0xD9; // 将原有的接受一个发送一个的标志翻转

                /*
                * #define SET55_BAUD 0xDA //计算0x55的波特率 #define REC_FR 0xE0 //接受一帧命令 E0
                * 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传， //待中断当前命令和中断处理命令，当接受的字节超过字节间的最大时间，自动正常退出。
                * //若设定了长度接受,超时最长等待时间,自动返回. #define REC_LEN 0xE1 //接受长度数据 E1-EF
                * 开始时回传开始接受信号，接受命令字节低四位为长度的数据自动退出，
                * //接到数据实时回传，待中断当前命令和中断处理命令，接受一个字节超过最长等待时间,正常退出. #define RECIEVE 0xF0
                * //连续接受 F0 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传， //直到接受中断当前命令和中断处理命令。
                */
                static const boost::uint8_t RUNLINK = 0xF1; // 启动链路保持 F1 启动链路保持，定时执行链路保持内容，在每次执行前回传链路保持开始信号，结束时回传链路保持结束信号。
                static const boost::uint8_t STOPLINK = 0xF2; // 中断链路保持 F2 结束链路保持执行。
                static const boost::uint8_t CLR_LINK = 0xDE; // 清除链路保持缓冲区
                static const boost::uint8_t DO_BAT_00 = 0xDF; // 批处理命令，执行一次命令缓冲区00地址的命令区
                // 3、多字节命令：（命令空间 0x30-= 0xCF）
                static const boost::uint8_t D0_BAT = 0x78; // 批处理命令，连续执行一次最多4块命令缓冲区的地址命令区；数据最多为4个命令区的首地址
                static const boost::uint8_t D0_BAT_FOR = 0x7C; // 批处理命令，连续执行无数次最多4块命令缓冲区的地址命令区；数据最多为4个命令区的首地址
                // 多字节命令
                static const boost::uint8_t SETTING = 0x80; // 下位机通讯链路状态字设定：设定3个通讯控制字，无用设定或没有设定都自动清零
                static const boost::uint8_t SETBAUD = 0x84; // 通讯波特率设定，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。

                /*
                * #define SETBYTETIME 0x88 //字节间时间设定 db20?（vpw为指令数）
                * ，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define SETWAITTIME 0x8c //空闲等待时间设定
                * db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define SETLINKTIME 0x90 //链路保持时建设定
                * db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define SETRECBBOUT 0x94
                * //接受字节超时错误判断 db20（vpw为指令数） ，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。 #define
                * SETRECFROUT 0x98 //接受一帧超时错误判断?db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                */
                static const boost::uint8_t ECHO = 0x9C; // 回传指定数据，按序回传数据
                static const boost::uint8_t SETPORT0 = 0xA0; // 只有一个字节的数据，设定端口0
                static const boost::uint8_t SETPORT1 = 0xA4; // 只有一个字节的数据，设定端口1
                static const boost::uint8_t SETPORT2 = 0xA8; // 只有一个字节的数据，设定端口2
                static const boost::uint8_t SETPORT3 = 0xAC; // 只有一个字节的数据，设定端口3
                // 已删除#define SETALLPORT 0x6F //只有四个字节的数据，设定端口0，1，2，3
                static const boost::uint8_t SET_UPBAUD = 0xB0; // 设置上位机的通讯波特率 ,仅有数据位1位,定义如下:其他非法

                /*
                * #define UP_9600BPS 0x00 #define UP_19200BPS 0x01 #define UP_38400BPS 0x02
                * #define UP_57600BPS 0x03 #define UP_115200BPS 0x04
                */
                static const boost::uint8_t DELAYSHORT = 0xB4; // 设定延时时间
                // (DB20)只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                static const boost::uint8_t DELAYTIME = 0xB8; // 设定延时时间
                // (DB20)只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                static const boost::uint8_t DELAYLONG = 0xBC; // 设定延时时间 (DB200)
                // 只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                // Operat Buff CMD
                // 指定修改 *
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
                static const boost::uint8_t GET_DATA = 0xC8; // 88 地址 返回数据缓冲区指定的数据
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
                //////////////////////////////////////////////////////////////////////////
                // 时间计算设置Commbox宏定义区
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t REPLAYTIMES = 3; // 错误运行次数
                //////////////////////////////////////////////////////////////////////////
                // 设置Commbox宏定义区
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint32_t TIMEVALUE = 1000000; // 万分之一秒微妙
                static const boost::uint8_t COMMBOXINFOLEN = 18; // 共有18个数据需从COMMBOX得到
                static const boost::uint8_t VERSIONLEN = 2;
                static const boost::uint8_t MINITIMELEN = 3;
                static const boost::uint8_t COMMBOXPORTNUM = 4;
                static const boost::uint8_t COMMBOXIDLEN = 10;
                static const boost::uint8_t LINKBLOCK = MAXIM_BLOCK; //链路保持的命令缓冲区
                // 批处理执行次数
                static const boost::uint8_t RUN_ONCE = 0x00;
                static const boost::uint8_t RUN_MORE = 0x01;
                // 通讯校验和方式
                static const boost::uint8_t CHECK_SUM = 0x01;
                static const boost::uint8_t CHECK_REVSUM = 0x02;
                static const boost::uint8_t CHECK_CRC = 0x03;
                // RF多对一的设定接口
                static const boost::uint8_t SETDTR_L = 0x02;
                static const boost::uint8_t SETDTR_H = 0x03;
                static const boost::uint8_t MAX_RFADD = 0x2F; // 0x00-= 0x2F间的= 0x30个地址
                static const boost::uint8_t SETADD = 0x10; // 切换无线通讯设备到新地址
                static const boost::uint8_t CHANGEADD = 0x40; // 改变当前与之通讯的无线设备的地址
                static const boost::uint8_t SETRFBAUD = 0x04; // 改变无线串口通讯波特率
                static const boost::uint8_t RESET_RF = 0x00; // 复位无线通讯主设备，该命令需在9600波特率下实现
                //////////////////////////////////////////////////////////////////////////
                // 通讯口 PORT
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t DH = 0x80; // 高电平输出,1为关闭,0为打开
                static const boost::uint8_t DL2 = 0x40; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线
                static const boost::uint8_t DL1 = 0x20; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制
                static const boost::uint8_t DL0 = 0x10; // 低电平输出,1为关闭,0为打开,正逻辑发送通讯线,带接受控制
                static const boost::uint8_t PWMS = 0x08; // PWM发送线
                static const boost::uint8_t PWMR = 0x04;
                static const boost::uint8_t COMS = 0x02; // 标准发送通讯线路
                static const boost::uint8_t COMR = 0x01;
                static const boost::uint8_t SET_NULL = 0x00; // 不选择任何设置
                //////////////////////////////////////////////////////////////////////////
                // 通讯物理控制口
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t PWC = 0x80; // 通讯电平控制,1为5伏,0为12伏
                static const boost::uint8_t REFC = 0x40; // 通讯比较电平控制,1为通讯电平1/5,0为比较电平控制1/2
                static const boost::uint8_t CK = 0x20; // K线控制开关,1为双线通讯,0为单线通讯
                static const boost::uint8_t SZFC = 0x10; // 发送逻辑控制,1为负逻辑,0为正逻辑
                static const boost::uint8_t RZFC = 0x08; // 接受逻辑控制,1为负逻辑,0为正逻辑
                static const boost::uint8_t DLC0 = 0x04; // DLC1接受控制,1为接受关闭,0为接受打开
                static const boost::uint8_t DLC1 = 0x02; // DLC0接受控制,1为接受关闭,0为接受打开
                static const boost::uint8_t SLC = 0x01; // 线选地址锁存器控制线(待用)
                static const boost::uint8_t CLOSEALL = 0x08; // 关闭所有发送口线，和接受口线
                //////////////////////////////////////////////////////////////////////////
                // 通讯控制字1设定
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
                // 通讯控制字3设定
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t ONEBYONE = 0x80;
                static const boost::uint8_t INVERTBYTE = 0x40;
                static const boost::uint8_t ORIGINALBYTE = 0x00;
                //////////////////////////////////////////////////////////////////////////
                // 通讯设置参数时间
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t SETBYTETIME = 0x88; // 字节间时间设定 db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                static const boost::uint8_t SETVPWSTART = 0x08; // 设置vpw发送数据时需发送0的时间。
                static const boost::uint8_t SETWAITTIME = 0x8C; // 空闲等待时间设定db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                static const boost::uint8_t SETLINKTIME = 0x90; // 链路保持时建设定db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                static const boost::uint8_t SETRECBBOUT = 0x94; // 接受字节超时错误判断 db20（vpw为指令数），只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                static const boost::uint8_t SETRECFROUT = 0x98; // 接受一帧超时错误判断?db20?，只用2个数据位，单字节为低字节，双字节高字节在前，低字节在后。
                static const boost::uint8_t SETVPWRECS = 0x14;
                //////////////////////////////////////////////////////////////////////////
                // 上下位机通讯波特率
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t UP_9600BPS = 0x0;
                static const boost::uint8_t UP_19200BPS = 0x01;
                static const boost::uint8_t UP_38400BPS = 0x02;
                static const boost::uint8_t UP_57600BPS = 0x03;
                static const boost::uint8_t UP_115200BPS = 0x04;
                //////////////////////////////////////////////////////////////////////////
                // 操作数据缓冲区
                //////////////////////////////////////////////////////////////////////////
                // 数据拷贝
                static const boost::uint8_t COPY_DATA = 0xCA; // 8A 结果地址1 操作地址1 长度 COPY 操作地址1 TO 结果地址1
                // FOR 长度 字节
                // 修改数据
                static const boost::uint8_t UPDATE_1BYTE = 0xC1; // 81 结果地址 数据1 结果地址=数据1
                static const boost::uint8_t UPDATE_2BYTE = 0xC3; // 83 结果地址1 数据1 结果地址2 数据2 结果地址1=数据1
                // 结果地址2=数据2
                // 自增命令
                static const boost::uint8_t DEC_DATA = 0xC4; // 84 结果地址 结果地址=结果地址-1
                static const boost::uint8_t INC_DATA = 0xC0; // 80 结果地址 结果地址=结果地址+1
                static const boost::uint8_t INC_2DATA = 0xC5; // 85 结果地址1 结果地址2 结果地址1=结果地址1+1
                // 结果地址2=结果地址2+1
                // 加法命令
                static const boost::uint8_t ADD_1BYTE = 0xC2; // 82 结果地址 操作地址1 数据1 结果地址=操作地址1+数据1
                static const boost::uint8_t ADD_2BYTE = 0xC7; // 87 结果地址1 数据1 结果地址2 数据2 结果地址1=结果地址1+数据1
                // 结果地址2=结果地址2+数据2
                static const boost::uint8_t ADD_DATA = 0xC6; // 86 结果地址1 操作地址1 操作地址2 结果地址1=操作地址1+操作地址2
                // 减法命令
                static const boost::uint8_t SUB_DATA = 0xCE; // 8E 结果地址1 操作地址1 操作地址2 结果地址1=操作地址1-操作地址2
                static const boost::uint8_t SUB_BYTE = 0xCD; // 8D 结果地址1 数据1 结果地址1=数据1-结果地址1
                static const boost::uint8_t INVERT_DATA = 0xCC; // 8C 结果地址1 结果地址1=~结果地址
                //////////////////////////////////////////////////////////////////////////
                // 接受命令类型定义
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t GET_PORT1 = 0xD8; // 等到通讯口的当前状态
                static const boost::uint8_t SET55_BAUD = 0xDA; // 计算= 0x55的波特率
                static const boost::uint8_t REC_FR = 0xE0; // 接受一帧命令 E0 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传，
                static const boost::uint8_t REC_LEN_1 = 0xE1; // 接受1个数据，返回
                static const boost::uint8_t REC_LEN_2 = 0xE2; // 接受2个数据，返回
                static const boost::uint8_t REC_LEN_3 = 0xE3; // 接受3个数据，返回
                static const boost::uint8_t REC_LEN_4 = 0xE4; // 接受4个数据，返回
                static const boost::uint8_t REC_LEN_5 = 0xE5; // 接受5个数据，返回
                static const boost::uint8_t REC_LEN_6 = 0xE6; // 接受6个数据，返回
                static const boost::uint8_t REC_LEN_7 = 0xE7; // 接受7个数据，返回
                static const boost::uint8_t REC_LEN_8 = 0xE8; // 接受8个数据，返回
                static const boost::uint8_t REC_LEN_9 = 0xE9; // 接受9个数据，返回
                static const boost::uint8_t REC_LEN_10 = 0xEA; // 接受10个数据，返回
                static const boost::uint8_t REC_LEN_11 = 0xEB; // 接受11个数据，返回
                static const boost::uint8_t REC_LEN_12 = 0xEC; // 接受12个数据，返回
                static const boost::uint8_t REC_LEN_13 = 0xED; // 接受13个数据，返回
                static const boost::uint8_t REC_LEN_14 = 0xEE; // 接受14个数据，返回
                static const boost::uint8_t REC_LEN_15 = 0xEF; // 接受15个数据，返回
                static const boost::uint8_t RECEIVE = 0xF0; // 连续接受 F0 开始时回传开始接受信号，然后长期等待接受，接到数据实时回传，
                //////////////////////////////////////////////////////////////////////////
                // ComBox记录信息和当前状态种类定义
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t GET_TIME = 0xF5; // 得到时间设定 DD返回字节时间、等待发送时间、链路保持时间、字节超时时间、接受超时时间
                static const boost::uint8_t GET_SET = 0xF6; // 得到链路设定 DE 返回链路控制字(3字节)、通讯波特率
                static const boost::uint8_t GET_PORT = 0xF7; // 得到端口设置 DF 返回端口p0，p1，p2，p3
                static const boost::uint8_t GET_LINKDATA = 0xF8; // 得到链路数据 FC 返回链路保持命令块中的所有内容 (中断命令)
                static const boost::uint8_t GET_BUFFDATA = 0xF9; // 得到缓冲器数据 FD 返回整个缓冲区数据 (中断命令)
                static const boost::uint8_t GET_COMMAND = 0xFA; // 得到命令数据 FE 返回上一执行命令。 (中断命令)
                //////////////////////////////////////////////////////////////////////////
                // 返回失败时，可根据Error_Record的值查找错误表定义
                //////////////////////////////////////////////////////////////////////////
                static const boost::uint8_t ILLIGICAL_LEN = 0xFF; //设置命令数据非法超长
                static const boost::uint8_t NOBUFF_TOSEND = 0xFE; //无交换缓冲区用于发送数据存放 
                static const boost::uint8_t SENDDATA_ERROR = 0xFD; //上位机发送数据异常
                static const boost::uint8_t CHECKSUM_ERROR = 0xFC; //接受命令回复校验和出错
                static const boost::uint8_t TIMEOUT_ERROR = 0xFB; //接受数据超时错误
                static const boost::uint8_t LOST_VERSIONDATA = 0xFA; //读到的Commbox数据长度不够.
                static const boost::uint8_t ILLIGICAL_CMD = 0xF9; //无此操作功能,没有定义.
                static const boost::uint8_t DISCONNECT_COMM = 0xF8; //没有连接上串口
                static const boost::uint8_t DISCONNECT_COMMBOX = 0xF7; //没有连接上COMMBOX设备
                static const boost::uint8_t NODEFINE_BUFF = 0xF6; //没有此命令块存在,未定义
                static const boost::uint8_t APPLICATION_NOW = 0xF5; //现有缓冲区申请,未取消,不能再此申请
                static const boost::uint8_t BUFFBUSING = 0xF4; //此缓冲区有数据未被撤销,不能使用,需删除此缓冲区,方可使用
                static const boost::uint8_t BUFFFLOW = 0xF3; //整个缓冲区无可使用的空间,不能申请,需删除缓冲区释放空间,方可使用
                static const boost::uint8_t NOAPPLICATBUFF = 0xF2; //未申请错误,需先申请,方可使用
                static const boost::uint8_t UNBUFF_CMD = 0xF1; //不是缓冲区命令,不能加载
                static const boost::uint8_t NOUSED_BUFF = 0xF0; //该缓冲区现没有使用,删除无效
                static const boost::uint8_t KEEPLINK_ERROR = 0xEF; //链路保持已断线
                static const boost::uint8_t UNDEFINE_CMD = 0xEE; //无效命令,未曾定义
                static const boost::uint8_t UNSET_EXRSBIT = 0xED; //没有设定扩展RS232的接受数据位个数
                static const boost::uint8_t COMMBAUD_OUT = 0xEC; //按照定义和倍增标志计算通讯波特率超出范围
                static const boost::uint8_t COMMTIME_OUT = 0xEB; //按照定义和倍增标志计算通讯时间超出范围
                static const boost::uint8_t OUTADDINBUFF = 0xEA; //缓冲区寻址越界
                static const boost::uint8_t COMMTIME_ZERO = 0xE9; //commbox时间基数为零
                static const boost::uint8_t SETTIME_ERROR = 0xE8; //延时时间为零
                static const boost::uint8_t NOADDDATA = 0xE7; //没有向申请的缓冲区填入命令,申请的缓冲区被撤销
                static const boost::uint8_t TESTNOLINK = 0xE6; //选择的线路没有连通
                static const boost::uint8_t PORTLEVELIDLE = 0xE5; //端口电平为常态
                static const boost::uint8_t COMMBOXID_ERR = 0xE4; //COMMBOX ID错误
                static const boost::uint8_t UP_TIMEOUT = 0xC0; //COMMBOX接受命令超时错误
                static const boost::uint8_t UP_DATAEER = 0xC1; //COMMBOX接受命令数据错误
                static const boost::uint8_t OVER_BUFF = 0xC2; //COMMBOX批处理缓冲区溢出,不判断链路保持数据是否会破坏缓冲区数据, 仅判断数据长度+数据地址>链路保持的开始位置成立溢出.*/
                static const boost::uint8_t ERROR_REC = 0xC3; //COMMBOX其他接受错误
                //COMMBOX执行操作错误
                static const boost::uint8_t SUCCESS = 0xAA; //COMMBOX执行成功
                static const boost::uint8_t SEND_OK = 0x55;
                static const boost::uint8_t RF_ERR = 0xC8;
                static const boost::uint8_t RUN_ERR = 0xC4; //COMMBOX运行启动检测错误
                //通讯通道定义
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
                static const boost::uint8_t SEND_CMD = 0x01;
            };
        }
    }
}

#endif