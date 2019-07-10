/**
 * @file DspOsdCmd.h
 * @brief
 * @author jerry.king
 * @date 2012-12-13 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef DSPOSDCMD_H_
#define DSPOSDCMD_H_
#include "iftOsdService.h"
/**
 * osd service info
 */
typedef enum E_OSD_SERVICE
{
    E_OSD_SERVICE_DSP_SNAP
} E_OSD_SERVICE;
typedef enum E_OSDSERV_STAT
{
    E_OSDSERV_STAT_ON,
    E_OSDSERV_STAT_OFF
} E_OSDSERV_STAT;
typedef enum E_DSPOSD_CMD
{
    E_DSPOSD_CMD_NULL, //useless cmd
    E_DSPOSD_CMD_ServStat,//get serv run status
    E_DSPOSD_CMD_DSPServReg, //regist one dsp service //such as OSD_MOD_DSP_SNAP
    E_DSPOSD_CMD_DSPServUnReg, //unregist one dsp service //such as OSD_MOD_DSP_SNAP
} E_DSPOSD_CMD;

#define DEF_OSDMSGPARM_VER 3
#define  MAX_PARAM_REQUEST 512
#define  MAX_PARAM_RESPONSE 1024
typedef enum E_OSDMSG_TYPE
{
    E_OSDMSG_REQUST,
    E_OSDMSG_RESPONSE
} E_OSDMSG_TYPE;
typedef struct SOsdMsg
{
    int32_t ver_t;
    E_DSPOSD_CMD cmd;
    E_OSDMSG_TYPE type; // need request(E_OSDMSG_REQUST) or (E_OSDMSG_RESPONSE)
    uint8_t req[MAX_PARAM_REQUEST];
    uint8_t resp[MAX_PARAM_RESPONSE];
} SOsdMsg;

//===================================================
///cmd  E_DSPOSD_ServGetStat
#define DEF_INVALID_USRID -1

//===================================================
//E_DSPOSD_CMD_DSPServReg, //regist one dsp service //such as OSD_MOD_DSP_SNAP
//request DSP-->ARM
typedef struct SOCMD_ServStat_req
{
    E_OSD_SERVICE serv;
} SOCMD_ServStat_req;

typedef struct SServUsrStat
{
    E_OSD_SERVICE serv;
    int32_t usrID; //alloc id for DSP
    E_OSDSERV_STAT stat;
    iftOsdServStyle style;
} SServUsrStat;

//response arm-->dsp
typedef struct SOCMD_ServStat_resp
{
    int32_t bResult; //0 false 1.ture
    SServUsrStat stat;
} SOCMD_ServStat_resp;
//===================================================
//===================================================
//E_DSPOSD_CMD_DSPServReg, //regist one dsp service //such as OSD_MOD_DSP_SNAP
//request DSP-->ARM
typedef struct SOCMD_DSPServReg_req
{
    E_OSD_SERVICE serv;
    int32_t usrID; // set DEF_INVALID_USRID  if do not know usrID
    iftOsdServStyle style;
} SOCMD_DSPServReg_req;
//response arm-->dsp
typedef struct SOCMD_DSPServReg_resp
{
    int32_t bResult; //0 false 1.ture
    int32_t usrID; //alloc id for DSP
    int8_t disp[256]; //response error description
} SOCMD_DSPServReg_resp;
//===================================================
//===================================================
//E_DSPOSD_CMD_DSPServUnReg, //unregist one dsp service //such as OSD_MOD_DSP_SNAP
//request DSP-->ARM
typedef struct SOCMD_DSPServUnReg_req
{
    E_OSD_SERVICE serv;
    int32_t usrID;
} SOCMD_DSPServUnReg_req;
//response arm-->dsp
typedef struct SOCMD_DSPServUnReg_resp
{
    int32_t bResult; //0 false 1.ture
    int32_t usrID;
} SOCMD_DSPServUnReg_resp;
//===================================================

#endif /* DSPOSDCMD_H_ */
