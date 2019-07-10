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
    NON_Encryption = 0              //没有加密
} EEncryption;



//SDK版本号定义

#ifndef MAKE_VER
typedef unsigned short  _VER_T;
#define MAKE_VER(a, b)  ((_VER_T)(((unsigned char)(((_VER_T)(a)) & 0xff)) | ((_VER_T)((unsigned char)(((_VER_T)(b)) & 0xff))) << 8))
#endif


typedef enum
{
    eNICPVer11 = MAKE_VER(1, 1)     //1.01版本
} ENICPVer;



typedef enum
{
    SET_UPDATE_INFO = 4022,         //发送升级命令信息
    GET_UPDATE_STATUS = 4023,       // 查询升级状态
    SET_UPDATE_RESTART = 4024,      //在升级结束后，发送重新启动命令
} ENICP_CMD;



//命令字执行结果定义
typedef enum
{
    SEND_CMD = 0,
    //SDK -> IPNC 0 - 9保留
    //IPNC -> SDK
    RECV_OK_CMD = 10,               //IPNC接收命令成功
    HANDLE_OK_CMD = 11,             //IPNC执行命令成功
    DATA_ERROR_CMD = -3,            //IPNC接收命令成功，但数据解析错误   可携带数据：字符串
    NOT_SUPPORT_CMD = -2,           //IPNC不支持该命令，无法解析    可携带数据：字符串
    HANDLE_ERROR_CMD = -1           //IPNC执行命令失败   可携带数据：字符串
} ENICP_CMD_RESULT;



//
//命令头结构定义
//

typedef struct ITE_NICP_HEAD
{
    ENICPVer            Ver;        //协议版本
    EEncryption         Encryption; //加密类型
    ENICP_CMD           Cmd;        //命令字
    unsigned int CmdIndex;   //命令序号
    //SDK请求命令包生成序号
    //IPNC返回命令处理结果包中填写对应序号
    ENICP_CMD_RESULT    Result;     //处理结果
    unsigned int         Len;        //数据长度
    unsigned int         Data;       //数据(小data区)
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
