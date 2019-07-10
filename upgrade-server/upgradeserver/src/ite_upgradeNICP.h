/**
 * @file ite_upgradeNICP.h
 * @brief
 * @author  <itarge@itarge.com>
 * @version 1.0.1
 * @date 2017-11-28
 */

/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __ITE_UPGTADENICP_H__
#define __ITE_UPGTADENICP_H__

typedef enum
{
    NON_Encryption = 0              //û�м���
} EEncryption;



//SDK�汾�Ŷ���

#ifndef MAKE_VER
typedef unsigned short  _VER_T;
#define MAKE_VER(a, b)  ((_VER_T)(((unsigned char)(((_VER_T)(a)) & 0xff)) | ((_VER_T)((unsigned char)(((_VER_T)(b)) & 0xff))) << 8))
#endif


typedef enum
{
    eNICPVer11 = MAKE_VER(1, 1)     //1.01�汾
} ENICPVer;



typedef enum
{
    SET_UPDATE_INFO = 4022,         //��������������Ϣ
    GET_UPDATE_STATUS = 4023,       // ��ѯ����״̬
    SET_UPDATE_RESTART = 4024,      //�����������󣬷���������������
} ENICP_CMD;



//������ִ�н������
typedef enum
{
    SEND_CMD = 0,
    //SDK -> IPNC 0 - 9����
    //IPNC -> SDK
    RECV_OK_CMD = 10,               //IPNC��������ɹ�
    HANDLE_OK_CMD = 11,             //IPNCִ������ɹ�
    DATA_ERROR_CMD = -3,            //IPNC��������ɹ��������ݽ�������   ��Я�����ݣ��ַ���
    NOT_SUPPORT_CMD = -2,           //IPNC��֧�ָ�����޷�����    ��Я�����ݣ��ַ���
    HANDLE_ERROR_CMD = -1           //IPNCִ������ʧ��   ��Я�����ݣ��ַ���
} ENICP_CMD_RESULT;



//
//����ͷ�ṹ����
//

typedef struct ITE_NICP_HEAD
{
    ENICPVer            Ver;        //Э��汾
    EEncryption         Encryption; //��������
    ENICP_CMD           Cmd;        //������
    unsigned int CmdIndex;   //�������
    //SDK����������������
    //IPNC�����������������д��Ӧ���
    ENICP_CMD_RESULT    Result;     //������
    unsigned int         Len;        //���ݳ���
    unsigned int         Data;       //����(Сdata��)
} ITE_NICP_HEAD, *PITE_NICP_HEAD;



//update sdk send to deviceinfo
typedef struct
{
    char    mode[4];//resv
    char    serverip[16];
    int     port;
    char    username[16];
    char    passwd[16];
    char    version[16];//resv
} RequestHeaderInfo;

typedef struct
{
    RequestHeaderInfo   header;
    OutFile_Info        updatefileinfo;
} SdkRequestUpdateInfo;
#endif /* __ITE_UPGTADENICP_H__ */
