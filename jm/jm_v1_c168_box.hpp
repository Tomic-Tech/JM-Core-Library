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
#include <jm_commbox_port.hpp>
#include <jm_v1_c168_d.hpp>

namespace jm {
namespace v1 {
class v1_c168_box : public v1_box<v1_c168_d> {
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
        uint8 add[D::MAXIM_BLOCK + 2];
        uint8 used[D::MAXIM_BLOCK];

        v1_buffer()
        : id(0)
        , used_num(0) {

        }

        uint32 size() {
            return add[D::LINKBLOCK] - add[D::SWAPBLOCK];
        }
    };

    struct v1_information {
        uint32 time_unit;
        uint32 time_base_db;
        uint32 time_extern_db;
        uint32 cmd_buff_len;
        uint8 version[D::VERSIONLEN];
        uint8 id[D::COMMBOXIDLEN];
        uint8 port[D::COMMBOXPORTNUM];
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

    v1_c168_box(const commbox_port_ptr &p, const v1_shared_ptr &s);
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
    size_type read_data(uint8 *buff, size_type offset, size_type count, int64 microseconds);
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
    bool add_to_buff(uint8 command_word, size_type offset, size_type count, const uint8 *data);
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
    bool set_echo_data(uint8 *echo_buff, size_type echo_len);
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
    bool send_out_data(const uint8 *buffer, size_type offset, size_type count);
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
    bool run_batch(uint8 *buff_id, size_type count, bool is_execute_many);
    size_type read_bytes(uint8 *buff, size_type offset, size_type count);
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
    bool commbox_do(uint8 command_word, size_type offset, size_type count, const uint8 *buff);
    bool send_data_to_ecu(uint8 command_word, size_type offset, size_type count, const uint8 *buff);
    bool send_data_to_ecu_new(uint8 command_word, size_type offset, size_type count, const uint8 *buff);
    bool send_data_to_ecu_old(uint8 command_word, size_type offset, size_type count, const uint8 *buff);
    bool commbox_command(uint8 command_word, size_type offset, size_type count, const uint8 *buff);
    bool do_set(uint8 command_word, size_type offset, size_type count, const uint8 *buff);
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
    size_type get_cmd_data(uint8 command, uint8 *receive_buffer);
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
    bool send_to_box(uint8 command_word, size_type offset, size_type count, const uint8 *buff);
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

typedef boost::shared_ptr<v1_c168_box> v1_c168_box_ptr;

}
}


#endif	/* JM_V1_C168_BOX_HPP */

