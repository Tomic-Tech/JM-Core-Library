#ifndef __JM_DIAG_V1_C168_CONSTANT_HPP__
#define __JM_DIAG_V1_C168_CONSTANT_HPP__

#ifndef _MSC_VER
#pragma once
#endif

#include <boost/cstdint.hpp>

namespace JM
{
namespace Diag
{
namespace V1
{
namespace C168
{
struct Constant
{
    //////////////////////////////////////////////////////////////////////////
    // CommBox �̶���Ϣ �궨���
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t NULLADD = 0xFF; // ��ʾ�˿���ʹ��
    static const boost::uint8_t MAXIM_BLOCK = 0x40; // ��������������
    static const boost::uint8_t SWAPBLOCK = (MAXIM_BLOCK + 1); // ���ݽ������Ŀ��ʶ
    static const boost::uint32_t START_BAUD = 57600; // ��λ��ͬ��λ��ͨ���ڸ�λ���ϵ�ʱ������Ϊ57600
    static const boost::uint8_t CMD_DATALEN = 4; // �Ƿ���������󳤶�
    // /////////////////////////////////////////////////////
    /*
    * // P1��ΪͨѶ�� #define DH 0x80 //�ߵ�ƽ���,1Ϊ�ر�,0Ϊ�� #define DL2 0x40
    * //�͵�ƽ���,1Ϊ�ر�,0Ϊ��,���߼�����ͨѶ�� #define DL1 0x20
    * //�͵�ƽ���,1Ϊ�ر�,0Ϊ��,���߼�����ͨѶ��,�����ܿ��� #define DL0 0x10
    * //�͵�ƽ���,1Ϊ�ر�,0Ϊ��,���߼�����ͨѶ��,�����ܿ��� #define PWMS 0x08 //PWM������ #define COMS
    * 0x02 //��׼����ͨѶ��· #define SET_NULL 0x00 //��ѡ���κ�����
    *
    * //P2��ΪͨѶ������ƿ� #define PWC 0x80 //ͨѶ��ƽ����,1Ϊ5��,0Ϊ12�� #define REFC 0x40
    * //ͨѶ�Ƚϵ�ƽ����,1ΪͨѶ��ƽ1/5,0Ϊ�Ƚϵ�ƽ����1/2 #define CK 0x20 //K�߿��ƿ���,1Ϊ˫��ͨѶ,0Ϊ����ͨѶ
    * #define SZFC 0x10 //�����߼�����,1Ϊ���߼�,0Ϊ���߼� #define RZFC 0x08
    * //�����߼�����,1Ϊ���߼�,0Ϊ���߼� #define DLC1 0x04 //DLC1���ܿ���,1Ϊ���ܹر�,0Ϊ���ܴ� #define
    * DLC0 0x02 //DLC0���ܿ���,1Ϊ���ܹر�,0Ϊ���ܴ� #define SLC 0x01 //��ѡ��ַ������������(����) //
    * P0��ѡ�߿��� #define CLOSEALL 0x08
    *
    * // ͨѶ�������趨 #define RS_232 0x00 //ͨѶ������1 #define EXRS_232 0x20 //ͨѶ������1
    * #define SET_VPW 0x40 //ͨѶ������1 #define SET_PWM 0x60 //ͨѶ������1 #define
    * BIT9_SPACE 0x00 //ͨѶ������1 #define BIT9_MARK 0x01 //ͨѶ������1 #define
    * BIT9_EVEN 0x02 //ͨѶ������1 #define BIT9_ODD 0x03 //ͨѶ������1 #define SEL_SL
    * 0x00 //ͨѶ������1 #define SEL_DL0 0x08 //ͨѶ������1 #define SEL_DL1 0x10 //ͨѶ������1
    * #define SEL_DL2 0x18 //ͨѶ������1 #define SET_DB20 0x04 //ͨѶ������1 #define
    * UN_DB20 0x00 //ͨѶ������1 #define ONEBYONE 0x80 //ͨѶ������3 #define INVERTBYTE
    * 0x40 //ͨѶ������3 #define ORIGNALBYTE 0X00 //ͨѶ������3
    */
    /***************************************************************************
    * �������: �����Ϊ����: 1��д������������ ��������õ�����������д�뻺��������ʽ���� ������ WR_DATA 0xD0?+
    * ���ȣ�����[N]+��ַ�� +д�뻺������ַ+����1+ ����2������+����N+У�顣 ��������N��Ϊ����У������У�鷽����ΪУ���
    * ��������Ÿ�ʽΪ�����ȣ�����[N]+��ַ�� +д�뻺������ַ+����1+ ����2������+����N 2�����ֽ����������д���������������
    * WR_DATA 0xD0����Ϊ���ֽ��������� ��ƿ��������ʽ���� ������+У��ͣ� �ǻ�������� �����ж�����2����ִֹͣ�У�ֹͣ����
    * �����λ���õ�����������ݣ��õ���·�������ݣ��õ��ϴλ�������������� ��������� 1�����������ݲ������ 2���������� 3����·��������
    * 4���������� 3�����ֽ����������ռ� 0x30-0xCF�� ��ʽ���£������֣�6BIT��+���ȣ����ݳ���-1��2BIT��+����[N]+У���
    * 1���������� 2�����ݲ�������
    *
    * 4���������������ռ� 0x00-0x2F�� ��ʽ���£� ���ȣ�����[N]+1��+����[N]+У���
    * ����������д�뻺����ʱ���ȿ�����0x2F����0x30�����ݣ�����д�뻺����ֱ�ӷ��ͣ�׷�಻����4�� 5���ж�����2����ִֹͣ�У�ֹͣ����
    * ���������֣���У�飬��Ϊһ���ֽڣ������з��أ��Եȴ����н����־���ء�
    ***************************************************************************/
    // 1��д������������
    static const boost::uint8_t WR_DATA = 0xD0; // д������������,д�����ݵ��������
    static const boost::uint8_t WR_LINK = 0xFF; // ��д������ĵ�ַΪWR_LINK��д�����ݵ���·��������·���������������������,��Ŵ���:����ַ�ӵ͵���
    static const boost::uint8_t SEND_LEN = 0xA8; // һ�η������ݵ����ݳ���,0X70������
    static const boost::uint8_t SEND_DATA = 0x00;
    // 2�����ֽ����������д��������������� WR_DATA 0xD0����Ϊ���ֽ���������
    // �ǻ���������
    static const boost::uint8_t RESET = 0xF3; // �����λ���� ������л������ͼĴ������ݡ�
    static const boost::uint8_t GETINFO = 0xF4;
    /*
    * �õ�CPU�ٶ� F9 ����CPU��ָ��ִ��ʱ�䣨������ƣ���ֵ���ݣ�3���ֽڣ� ��ʱ����Ʋ��� ����ʱ����Ƶ�ָ��ִ������
    * �������ƣ�1byte����DB20�� ���ȴ����ƣ�1byte����DB200�� ���������ȣ�1byte�� ��Ʒ��ţ�10byte�� �Ͱ汾��Ϣ
    * ����Commbox��Ӳ���汾�š� �ȴ�����5�ֽ����룺��������ֽ�ΪУ��ͣ�ͬ��Կѭ������У��ͣ�����������ֵ��
    */
    /*
    * #define GET_TIME 0xF5 //�õ�ʱ���趨 DD �����ֽ�ʱ�䡢�ȴ�����ʱ�䡢��·����ʱ�䡢�ֽڳ�ʱʱ�䡢���ܳ�ʱʱ��
    * #define GET_SET 0xF6 //�õ���·�趨 DE ������·������(3�ֽ�)��ͨѶ������ #define GET_PORT 0xF7
    * //�õ��˿����� DF ���ض˿�p0��p1��p2��p3 #define GET_LINKDATA 0xF8 //�õ���·���� FC
    * ������·����������е��������� (�ж�����) #define GET_BUFFDATA 0xF9 //�õ����������� FD ������������������
    * (�ж�����) #define GET_CMMAND 0xFA //�õ��������� FE ������һִ����� (�ж�����)
    */
    // �ж������
    static const boost::uint8_t STOP_REC = 0xFB; // �жϽ������� ǿ���˳���ǰ������������ش���(�ж�����)
    static const boost::uint8_t STOP_EXECUTE = 0xFC; // �ж�����������
    // �ڵ�ǰִ��ʱ��ͨ��������ֹͣ��ǰ���ܲ��������ش���(�ж�����)
    // ���ֽڻ���������
    // static const boost::uint8_t GET_PORT1 0xD8 //�ȵ�ͨѶ�ڵĵ�ǰ״̬
    static const boost::uint8_t SET_ONEBYONE = 0xD9; // ��ԭ�еĽ���һ������һ���ı�־��ת

    /*
    * #define SET55_BAUD 0xDA //����0x55�Ĳ����� #define REC_FR 0xE0 //����һ֡���� E0
    * ��ʼʱ�ش���ʼ�����źţ�Ȼ���ڵȴ����ܣ��ӵ�����ʵʱ�ش��� //���жϵ�ǰ������жϴ�����������ܵ��ֽڳ����ֽڼ�����ʱ�䣬�Զ������˳���
    * //���趨�˳��Ƚ���,��ʱ��ȴ�ʱ��,�Զ�����. #define REC_LEN 0xE1 //���ܳ������� E1-EF
    * ��ʼʱ�ش���ʼ�����źţ����������ֽڵ���λΪ���ȵ������Զ��˳���
    * //�ӵ�����ʵʱ�ش������жϵ�ǰ������жϴ����������һ���ֽڳ�����ȴ�ʱ��,�����˳�. #define RECIEVE 0xF0
    * //�������� F0 ��ʼʱ�ش���ʼ�����źţ�Ȼ���ڵȴ����ܣ��ӵ�����ʵʱ�ش��� //ֱ�������жϵ�ǰ������жϴ������
    */
    static const boost::uint8_t RUNLINK = 0xF1; // ������·���� F1 ������·���֣���ʱִ����·�������ݣ���ÿ��ִ��ǰ�ش���·���ֿ�ʼ�źţ�����ʱ�ش���·���ֽ����źš�
    static const boost::uint8_t STOPLINK = 0xF2; // �ж���·���� F2 ������·����ִ�С�
    static const boost::uint8_t CLR_LINK = 0xDE; // �����·���ֻ�����
    static const boost::uint8_t DO_BAT_00 = 0xDF; // ���������ִ��һ���������00��ַ��������
    // 3�����ֽ����������ռ� 0x30-= 0xCF��
    static const boost::uint8_t D0_BAT = 0x78; // �������������ִ��һ�����4����������ĵ�ַ���������������Ϊ4�����������׵�ַ
    static const boost::uint8_t D0_BAT_FOR = 0x7C; // �������������ִ�����������4����������ĵ�ַ���������������Ϊ4�����������׵�ַ
    // ���ֽ�����
    static const boost::uint8_t SETTING = 0x80; // ��λ��ͨѶ��·״̬���趨���趨3��ͨѶ�����֣������趨��û���趨���Զ�����
    static const boost::uint8_t SETBAUD = 0x84; // ͨѶ�������趨��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�

    /*
    * #define SETBYTETIME 0x88 //�ֽڼ�ʱ���趨 db20?��vpwΪָ������
    * ��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں� #define SETWAITTIME 0x8c //���еȴ�ʱ���趨
    * db20?��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں� #define SETLINKTIME 0x90 //��·����ʱ���趨
    * db20?��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں� #define SETRECBBOUT 0x94
    * //�����ֽڳ�ʱ�����ж� db20��vpwΪָ������ ��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں� #define
    * SETRECFROUT 0x98 //����һ֡��ʱ�����ж�?db20?��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    */
    static const boost::uint8_t ECHO = 0x9C; // �ش�ָ�����ݣ�����ش�����
    static const boost::uint8_t SETPORT0 = 0xA0; // ֻ��һ���ֽڵ����ݣ��趨�˿�0
    static const boost::uint8_t SETPORT1 = 0xA4; // ֻ��һ���ֽڵ����ݣ��趨�˿�1
    static const boost::uint8_t SETPORT2 = 0xA8; // ֻ��һ���ֽڵ����ݣ��趨�˿�2
    static const boost::uint8_t SETPORT3 = 0xAC; // ֻ��һ���ֽڵ����ݣ��趨�˿�3
    // ��ɾ��#define SETALLPORT 0x6F //ֻ���ĸ��ֽڵ����ݣ��趨�˿�0��1��2��3
    static const boost::uint8_t SET_UPBAUD = 0xB0; // ������λ����ͨѶ������ ,��������λ1λ,��������:�����Ƿ�

    /*
    * #define UP_9600BPS 0x00 #define UP_19200BPS 0x01 #define UP_38400BPS 0x02
    * #define UP_57600BPS 0x03 #define UP_115200BPS 0x04
    */
    static const boost::uint8_t DELAYSHORT = 0xB4; // �趨��ʱʱ��
    // (DB20)ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    static const boost::uint8_t DELAYTIME = 0xB8; // �趨��ʱʱ��
    // (DB20)ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    static const boost::uint8_t DELAYLONG = 0xBC; // �趨��ʱʱ�� (DB200)
    // ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    // Operat Buff CMD
    // ָ���޸� *
    /*
    * #define UPDATE_1BYTE 0xc1 //81 �����ַ ����1 �����ַ=����1 #define UPDATE_2BYTE
    * 0xc3 //83 �����ַ1 ����1 �����ַ2 ����2 �����ַ1=����1 �����ַ2=����2 //���ݿ��� #define
    * COPY_DATA 0xcA //8A �����ַ1 ������ַ1 ���� COPY ������ַ1 TO �����ַ1 FOR ���� �ֽ� //��������
    * #define DEC_DATA 0xc4 //84 �����ַ �����ַ=�����ַ-1 #define INC_DATA 0xc0 //80
    * �����ַ �����ַ=�����ַ+1 #define INC_2DATA 0xc5 //85 �����ַ1 �����ַ2 �����ַ1=�����ַ1+1
    * �����ַ2=�����ַ2+1 //�ӷ����� #define ADD_1BYTE 0xc2 //82 �����ַ ������ַ1 ����1
    * �����ַ=������ַ1+����1 #define ADD_2BYTE 0xc7 //87 �����ַ1 �����ַ2 ����1 ����2
    * �����ַ1=�����ַ1+����1 �����ַ2=�����ַ2+����2 #define ADD_DATA 0xc6 //86 �����ַ1 ������ַ1
    * ������ַ2 �����ַ1=������ַ1+������ַ2 //�������� #define SUB_DATA 0xce //8E �����ַ1 ������ַ1
    * ������ַ2 �����ַ1=������ַ1-������ַ2 #define SUB_BYTE 0xcD //8D �����ַ1 ����1
    * �����ַ1=����1-�����ַ1 #define INVERT_DATA 0xcC //8C �����ַ1 �����ַ1=~�����ַ //ȡ����
    * #define GET_NDATA 0xc9 //88 ��ַ �������ݻ�����ָ��������
    *
    * #define UPDATE_1BYTE_A 0xc0 //81 �����ַ ����1 �����ַ=����1 #define UPDATE_2BYTE_A
    * 0xc0 //83 �����ַ1 ����1 �����ַ2 ����2 �����ַ1=����1 �����ַ2=����2 //�������� #define
    * DEC_DATA_A 0xc4 //84 �����ַ �����ַ=�����ַ-1 #define INC_DATA_A 0xc0 //80 �����ַ
    * �����ַ=�����ַ+1 #define INC_2DATA_A 0xc4 //85 �����ַ1 �����ַ2 �����ַ1=�����ַ1+1
    * �����ַ2=�����ַ2+1 //�ӷ����� #define ADD_1BYTE_A 0xc0 //82 �����ַ ������ַ1 ����1
    * �����ַ=������ַ1+����1 #define ADD_2BYTE_A 0xc4 //87 �����ַ1 �����ַ2 ����1 ����2
    * �����ַ1=�����ַ1+����1 �����ַ2=�����ַ2+����2 #define ADD_DATA_A 0xc4 //86 �����ַ1 ������ַ1
    * ������ַ2 �����ַ1=������ַ1+������ַ2 //�������� #define SUB_DATA_A 0xcc //8E �����ַ1 ������ַ1
    * ������ַ2 �����ַ1=������ַ1-������ַ2 #define SUB_BYTE_A 0xcc //8D �����ַ1 ����1
    * �����ַ1=����1-�����ַ1 #define INVERT_DATA_A 0xcC //8C �����ַ1 �����ַ1=~�����ַ
    */
    // ȡ����
    static const boost::uint8_t GET_DATA = 0xC8; // 88 ��ַ �������ݻ�����ָ��������
    /***************************************************************************
    * �����������: ���������Ϊ����: 1 ���ֽڷ���:�޳��Ⱥ�У��,�����ص��ֽ� 1 ����,�ɹ���Ϣ: 2 ���ܵ�����:(��������,ͨѶ�˿�����)
    * ʹ���ڻ��������� 2 ���ֽڷ���: 1 ��ʽ:���ܵ������� + ���� + ���� + У��� ���ȣ����������ݸ��� ʹ���ڷǻ��������� 3
    * �ж�������أ���ִ�н������
    ***************************************************************************/
    /*
    * // 1 ���ֽڷ���:�޳��Ⱥ�У��,�����ص��ֽ� //���ܷ��ش�����Ϣ���� #define UP_TIMEOUT 0xC0 //�������ʱ����
    * #define UP_DATAEER 0xC1 //�����������ݴ��� #define OVER_BUFF 0xC2
    * //�������������,���ж���·���������Ƿ���ƻ�����������, //���ж����ݳ���+���ݵ�ַ>��·���ֵĿ�ʼλ�ó������. #define
    * ERROR_REC 0xC3 //�������ܴ���
    *
    * //ִ�в������� #define SUCCESS 0xAA //ִ�гɹ� #define RUN_ERR 0xC4 //��������������
    */
    //////////////////////////////////////////////////////////////////////////
    // ʱ���������Commbox�궨����
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t REPLAYTIMES = 3; // �������д���
    //////////////////////////////////////////////////////////////////////////
    // ����Commbox�궨����
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint32_t TIMEVALUE = 1000000; // ���֮һ��΢��
    static const boost::uint8_t COMMBOXINFOLEN = 18; // ����18���������COMMBOX�õ�
    static const boost::uint8_t VERSIONLEN = 2;
    static const boost::uint8_t MINITIMELEN = 3;
    static const boost::uint8_t COMMBOXPORTNUM = 4;
    static const boost::uint8_t COMMBOXIDLEN = 10;
    static const boost::uint8_t LINKBLOCK = MAXIM_BLOCK; //��·���ֵ��������
    // ������ִ�д���
    static const boost::uint8_t RUN_ONCE = 0x00;
    static const boost::uint8_t RUN_MORE = 0x01;
    // ͨѶУ��ͷ�ʽ
    static const boost::uint8_t CHECK_SUM = 0x01;
    static const boost::uint8_t CHECK_REVSUM = 0x02;
    static const boost::uint8_t CHECK_CRC = 0x03;
    // RF���һ���趨�ӿ�
    static const boost::uint8_t SETDTR_L = 0x02;
    static const boost::uint8_t SETDTR_H = 0x03;
    static const boost::uint8_t MAX_RFADD = 0x2F; // 0x00-= 0x2F���= 0x30����ַ
    static const boost::uint8_t SETADD = 0x10; // �л�����ͨѶ�豸���µ�ַ
    static const boost::uint8_t CHANGEADD = 0x40; // �ı䵱ǰ��֮ͨѶ�������豸�ĵ�ַ
    static const boost::uint8_t SETRFBAUD = 0x04; // �ı����ߴ���ͨѶ������
    static const boost::uint8_t RESET_RF = 0x00; // ��λ����ͨѶ���豸������������9600��������ʵ��
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
    // ͨѶ������ƿ�
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
    static const boost::uint8_t ORIGINALBYTE = 0x00;
    //////////////////////////////////////////////////////////////////////////
    // ͨѶ���ò���ʱ��
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t SETBYTETIME = 0x88; // �ֽڼ�ʱ���趨 db20?��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    static const boost::uint8_t SETVPWSTART = 0x08; // ����vpw��������ʱ�跢��0��ʱ�䡣
    static const boost::uint8_t SETWAITTIME = 0x8C; // ���еȴ�ʱ���趨db20?��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    static const boost::uint8_t SETLINKTIME = 0x90; // ��·����ʱ���趨db20?��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    static const boost::uint8_t SETRECBBOUT = 0x94; // �����ֽڳ�ʱ�����ж� db20��vpwΪָ��������ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    static const boost::uint8_t SETRECFROUT = 0x98; // ����һ֡��ʱ�����ж�?db20?��ֻ��2������λ�����ֽ�Ϊ���ֽڣ�˫�ֽڸ��ֽ���ǰ�����ֽ��ں�
    static const boost::uint8_t SETVPWRECS = 0x14;
    //////////////////////////////////////////////////////////////////////////
    // ����λ��ͨѶ������
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t UP_9600BPS = 0x0;
    static const boost::uint8_t UP_19200BPS = 0x01;
    static const boost::uint8_t UP_38400BPS = 0x02;
    static const boost::uint8_t UP_57600BPS = 0x03;
    static const boost::uint8_t UP_115200BPS = 0x04;
    //////////////////////////////////////////////////////////////////////////
    // �������ݻ�����
    //////////////////////////////////////////////////////////////////////////
    // ���ݿ���
    static const boost::uint8_t COPY_DATA = 0xCA; // 8A �����ַ1 ������ַ1 ���� COPY ������ַ1 TO �����ַ1
    // FOR ���� �ֽ�
    // �޸�����
    static const boost::uint8_t UPDATE_1BYTE = 0xC1; // 81 �����ַ ����1 �����ַ=����1
    static const boost::uint8_t UPDATE_2BYTE = 0xC3; // 83 �����ַ1 ����1 �����ַ2 ����2 �����ַ1=����1
    // �����ַ2=����2
    // ��������
    static const boost::uint8_t DEC_DATA = 0xC4; // 84 �����ַ �����ַ=�����ַ-1
    static const boost::uint8_t INC_DATA = 0xC0; // 80 �����ַ �����ַ=�����ַ+1
    static const boost::uint8_t INC_2DATA = 0xC5; // 85 �����ַ1 �����ַ2 �����ַ1=�����ַ1+1
    // �����ַ2=�����ַ2+1
    // �ӷ�����
    static const boost::uint8_t ADD_1BYTE = 0xC2; // 82 �����ַ ������ַ1 ����1 �����ַ=������ַ1+����1
    static const boost::uint8_t ADD_2BYTE = 0xC7; // 87 �����ַ1 ����1 �����ַ2 ����2 �����ַ1=�����ַ1+����1
    // �����ַ2=�����ַ2+����2
    static const boost::uint8_t ADD_DATA = 0xC6; // 86 �����ַ1 ������ַ1 ������ַ2 �����ַ1=������ַ1+������ַ2
    // ��������
    static const boost::uint8_t SUB_DATA = 0xCE; // 8E �����ַ1 ������ַ1 ������ַ2 �����ַ1=������ַ1-������ַ2
    static const boost::uint8_t SUB_BYTE = 0xCD; // 8D �����ַ1 ����1 �����ַ1=����1-�����ַ1
    static const boost::uint8_t INVERT_DATA = 0xCC; // 8C �����ַ1 �����ַ1=~�����ַ
    //////////////////////////////////////////////////////////////////////////
    // �����������Ͷ���
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t GET_PORT1 = 0xD8; // �ȵ�ͨѶ�ڵĵ�ǰ״̬
    static const boost::uint8_t SET55_BAUD = 0xDA; // ����= 0x55�Ĳ�����
    static const boost::uint8_t REC_FR = 0xE0; // ����һ֡���� E0 ��ʼʱ�ش���ʼ�����źţ�Ȼ���ڵȴ����ܣ��ӵ�����ʵʱ�ش���
    static const boost::uint8_t REC_LEN_1 = 0xE1; // ����1�����ݣ�����
    static const boost::uint8_t REC_LEN_2 = 0xE2; // ����2�����ݣ�����
    static const boost::uint8_t REC_LEN_3 = 0xE3; // ����3�����ݣ�����
    static const boost::uint8_t REC_LEN_4 = 0xE4; // ����4�����ݣ�����
    static const boost::uint8_t REC_LEN_5 = 0xE5; // ����5�����ݣ�����
    static const boost::uint8_t REC_LEN_6 = 0xE6; // ����6�����ݣ�����
    static const boost::uint8_t REC_LEN_7 = 0xE7; // ����7�����ݣ�����
    static const boost::uint8_t REC_LEN_8 = 0xE8; // ����8�����ݣ�����
    static const boost::uint8_t REC_LEN_9 = 0xE9; // ����9�����ݣ�����
    static const boost::uint8_t REC_LEN_10 = 0xEA; // ����10�����ݣ�����
    static const boost::uint8_t REC_LEN_11 = 0xEB; // ����11�����ݣ�����
    static const boost::uint8_t REC_LEN_12 = 0xEC; // ����12�����ݣ�����
    static const boost::uint8_t REC_LEN_13 = 0xED; // ����13�����ݣ�����
    static const boost::uint8_t REC_LEN_14 = 0xEE; // ����14�����ݣ�����
    static const boost::uint8_t REC_LEN_15 = 0xEF; // ����15�����ݣ�����
    static const boost::uint8_t RECEIVE = 0xF0; // �������� F0 ��ʼʱ�ش���ʼ�����źţ�Ȼ���ڵȴ����ܣ��ӵ�����ʵʱ�ش���
    //////////////////////////////////////////////////////////////////////////
    // ComBox��¼��Ϣ�͵�ǰ״̬���ඨ��
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t GET_TIME = 0xF5; // �õ�ʱ���趨 DD�����ֽ�ʱ�䡢�ȴ�����ʱ�䡢��·����ʱ�䡢�ֽڳ�ʱʱ�䡢���ܳ�ʱʱ��
    static const boost::uint8_t GET_SET = 0xF6; // �õ���·�趨 DE ������·������(3�ֽ�)��ͨѶ������
    static const boost::uint8_t GET_PORT = 0xF7; // �õ��˿����� DF ���ض˿�p0��p1��p2��p3
    static const boost::uint8_t GET_LINKDATA = 0xF8; // �õ���·���� FC ������·����������е��������� (�ж�����)
    static const boost::uint8_t GET_BUFFDATA = 0xF9; // �õ����������� FD ������������������ (�ж�����)
    static const boost::uint8_t GET_COMMAND = 0xFA; // �õ��������� FE ������һִ����� (�ж�����)
    //////////////////////////////////////////////////////////////////////////
    // ����ʧ��ʱ���ɸ���Error_Record��ֵ���Ҵ������
    //////////////////////////////////////////////////////////////////////////
    static const boost::uint8_t ILLIGICAL_LEN = 0xFF; //�����������ݷǷ�����
    static const boost::uint8_t NOBUFF_TOSEND = 0xFE; //�޽������������ڷ������ݴ��
    static const boost::uint8_t SENDDATA_ERROR = 0xFD; //��λ�����������쳣
    static const boost::uint8_t CHECKSUM_ERROR = 0xFC; //��������ظ�У��ͳ���
    static const boost::uint8_t TIMEOUT_ERROR = 0xFB; //�������ݳ�ʱ����
    static const boost::uint8_t LOST_VERSIONDATA = 0xFA; //������Commbox���ݳ��Ȳ���.
    static const boost::uint8_t ILLIGICAL_CMD = 0xF9; //�޴˲�������,û�ж���.
    static const boost::uint8_t DISCONNECT_COMM = 0xF8; //û�������ϴ���
    static const boost::uint8_t DISCONNECT_COMMBOX = 0xF7; //û��������COMMBOX�豸
    static const boost::uint8_t NODEFINE_BUFF = 0xF6; //û�д���������,δ����
    static const boost::uint8_t APPLICATION_NOW = 0xF5; //���л���������,δȡ��,�����ٴ�����
    static const boost::uint8_t BUFFBUSING = 0xF4; //�˻�����������δ������,����ʹ��,��ɾ���˻�����,����ʹ��
    static const boost::uint8_t BUFFFLOW = 0xF3; //�����������޿�ʹ�õĿռ�,��������,��ɾ���������ͷſռ�,����ʹ��
    static const boost::uint8_t NOAPPLICATBUFF = 0xF2; //δ�������,��������,����ʹ��
    static const boost::uint8_t UNBUFF_CMD = 0xF1; //���ǻ���������,���ܼ���
    static const boost::uint8_t NOUSED_BUFF = 0xF0; //�û�������û��ʹ��,ɾ����Ч
    static const boost::uint8_t KEEPLINK_ERROR = 0xEF; //��·�����Ѷ���
    static const boost::uint8_t UNDEFINE_CMD = 0xEE; //��Ч����,δ������
    static const boost::uint8_t UNSET_EXRSBIT = 0xED; //û���趨��չRS232�Ľ�������λ����
    static const boost::uint8_t COMMBAUD_OUT = 0xEC; //���ն���ͱ�����־����ͨѶ�����ʳ�����Χ
    static const boost::uint8_t COMMTIME_OUT = 0xEB; //���ն���ͱ�����־����ͨѶʱ�䳬����Χ
    static const boost::uint8_t OUTADDINBUFF = 0xEA; //������ѰַԽ��
    static const boost::uint8_t COMMTIME_ZERO = 0xE9; //commboxʱ�����Ϊ��
    static const boost::uint8_t SETTIME_ERROR = 0xE8; //��ʱʱ��Ϊ��
    static const boost::uint8_t NOADDDATA = 0xE7; //û��������Ļ�������������,����Ļ�����������
    static const boost::uint8_t TESTNOLINK = 0xE6; //ѡ�����·û����ͨ
    static const boost::uint8_t PORTLEVELIDLE = 0xE5; //�˿ڵ�ƽΪ��̬
    static const boost::uint8_t COMMBOXID_ERR = 0xE4; //COMMBOX ID����
    static const boost::uint8_t UP_TIMEOUT = 0xC0; //COMMBOX�������ʱ����
    static const boost::uint8_t UP_DATAEER = 0xC1; //COMMBOX�����������ݴ���
    static const boost::uint8_t OVER_BUFF = 0xC2; //COMMBOX�������������,���ж���·���������Ƿ���ƻ�����������, ���ж����ݳ���+���ݵ�ַ>��·���ֵĿ�ʼλ�ó������.*/
    static const boost::uint8_t ERROR_REC = 0xC3; //COMMBOX�������ܴ���
    //COMMBOXִ�в�������
    static const boost::uint8_t SUCCESS = 0xAA; //COMMBOXִ�гɹ�
    static const boost::uint8_t SEND_OK = 0x55;
    static const boost::uint8_t RF_ERR = 0xC8;
    static const boost::uint8_t RUN_ERR = 0xC4; //COMMBOX��������������
    //ͨѶͨ������
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
}

#endif